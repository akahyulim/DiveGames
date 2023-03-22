#include "divepch.h"
#include "View.h"
#include "Viewport.h"
#include "Renderer.h"
#include "Batch.h"
#include "RenderPath.h"
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Technique.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTexture.h"
#include "Resource/ResourceCache.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Drawable.h"
#include "Scene/Component/Light.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"
#include "IO/FileSystem.h"
#include "Math/Math.h"

namespace Dive 
{
	View::View(Context* pContext)
		: Object(pContext),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pRenderer(GetSubsystem<Renderer>()),
		m_pScene(nullptr),
		m_pCamera(nullptr),
		m_pRenderPath(nullptr),
		m_pRenderTarget(nullptr),
		m_pCurRenderTarget(nullptr),
		m_ViewRect({ 0,0,0,0 }),
		m_ViewSize(0, 0),
		m_RenderTargetSize(0, 0)
	{
		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	View::~View()
	{
		DV_LOG_ENGINE_TRACE("View 소멸 완료");
	}

	void View::Update(float delta)
	{
		// 실제로는 octree와 cullCamera 이다.
		if (!m_pScene)
			return;

		m_Drawables.clear();
		m_Lights.clear();
		for (auto& batchQueue : m_BatchQueues)
			batchQueue.second.Clear();

		getDrawables();
		getBaseBatches();
		getLightBatches();
	}
	
	void View::Render()
	{
		// UpdateGeometries
		// 여기에서 queue를 sort를 한다.

		executeRenderPathCommands();
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderPath = pViewport->GetRenderPath();
		if (!m_pRenderPath)
			return false;

		m_pRenderTarget = pRenderTarget;
		m_pScene = pViewport->GetScene();
		m_pCamera = pViewport->GetCamera();

		int width = pRenderTarget ? pRenderTarget->GetWidth() : m_pGraphics->GetWidth();
		int height = pRenderTarget ? pRenderTarget->GetHeight() : m_pGraphics->GetHeight();
		m_pCamera->SetAspectRatio(static_cast<float>(width), static_cast<float>(height));
		m_RenderTargetSize = DirectX::XMINT2(width, height);

		RECT rect = pViewport->GetRect();
		if (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0)
		{
			m_ViewRect.left = 0;
			m_ViewRect.right = width;
			m_ViewRect.top = 0;
			m_ViewRect.bottom = height;
		}
		else
		{
			m_ViewRect.left = Math::Clamp<int>(rect.left, 0, width - 1);
			m_ViewRect.right = Math::Clamp<int>(rect.right, m_ViewRect.left + 1, width);
			m_ViewRect.top = Math::Clamp<int>(rect.top, 0, height - 1);
			m_ViewRect.bottom = Math::Clamp<int>(rect.bottom, m_ViewRect.top + 1, height);
		}
		m_ViewSize = DirectX::XMINT2(m_ViewRect.right - m_ViewRect.left, m_ViewRect.bottom - m_ViewRect.top);

		// 각종 초기화
		m_ScenePasses.clear();

		// ScenePass Command의 일부정보로 m_ScenePasses 구성
		for(uint32_t i = 0; i < m_pRenderPath->GetCommandCount(); ++i)
		{
			RenderPathCommand* pCommand = m_pRenderPath->GetCommand(i);
			// unabled 확인

			// 결국 다른 패스에서는 m_ScenePasses와 m_BatchQueues가 구성되지 않는다.
			// 라이트의 경우 m_LightQueues를 사용하니 구성 과정도 구분되는 것 같다.
			if (pCommand->Type == eRenderCommandType::ScenePass)
			{
				ScenePassInfo info;
				info.PassIndex = pCommand->PassIndex = Technique::GetPassIndex(pCommand->Pass);
				info.bAllowInstancing = false;	// command.sortMode != SORT_BACKTOFRONT
				info.bMarkStencil;
				info.bVertexLight = pCommand->bVertexLights;

				// 현재 Pass의 m_BatchQueues와 m_ScenePassed의 BatchQueue를 연결
				auto it = m_BatchQueues.find(info.PassIndex);
				if (it == m_BatchQueues.end())
					m_BatchQueues.emplace(info.PassIndex, BatchQueue());
				info.pBatchQueue = &m_BatchQueues[info.PassIndex];

				m_ScenePasses.emplace_back(info);
			}
		}

		// 이외에도 남은 처리가 존재한다.

		return true;
	}

	void View::getDrawables()
	{
		// 일단 그려져야 할 오브젝트부터 추린다.
		// 옥트리와 컬링을 거친 오브젝트일 듯 하다.
		// 그런데 Light도 Drawable일까?
		// 실제로 Light가 Drwable을 상속했다... 굳이 따라할 필요는 없지 않나?
		// => 하지만 한 곳에서 분류하는 것이 더 낫긴하다.


		auto allGameObjects = m_pScene->GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			// 이러면 SkinnedDrawable은 안될텐데...
			// 미래의 내가 고치겠지.
			if (pGameObject->HasComponent<Dive::Drawable>())
				m_Drawables.emplace_back(pGameObject->GetComponent<Dive::Drawable>());

			if (pGameObject->HasComponent<Dive::Light>())
				m_Lights.emplace_back(pGameObject->GetComponent<Dive::Light>());
		}
	}

	void View::getBaseBatches()
	{
		for(const auto* pDrawable : m_Drawables)
		{
			const auto& sourceDatas = pDrawable->GetSourceDatas();

			for (uint32_t i = 0; i < static_cast<uint32_t>(sourceDatas.size()); ++i)
			{
				const DrawableSourceData& drawableBatch = sourceDatas[i];
				
				// 여기에서 Material이 없다면 에러가 발생한다.
				// urho는 GetTechnique이라는 함수를 만들어 drawable과 material을 전달하여 technique을 리턴받도록 했다.
				Technique* pTech = drawableBatch.pMaterial->GetTechnique();
				if (!pTech)
					continue;

				// 이게 BaseBatche의 특이점이다.
				// Pass별로 BatchQueue를 구성하고 있다.
				// 즉, 하나의 Mesh도 Pass에 따라 다양한 형태의 Shading을 적용할 수 있다는 것이다.
				for(uint32_t j = 0; j < static_cast<uint32_t>(m_ScenePasses.size()); ++j)
				{
					ScenePassInfo& info = m_ScenePasses[j];

					// base index라는 걸 skip한다.. 아직 이해 불가.

					Pass* pPass = pTech->GetPass(info.PassIndex);
					if (!pPass)
						continue;

					StaticBatch batch(drawableBatch);
					batch.SetPass(pPass);

					// vertexLight와 vertexColor은 다르다. 혼동한 듯 하다.
					if (info.bVertexLight)
					{
						// light를 찾아 등록한다. 자세히는 모르겠다.
					}
					else
					{
						// 여긴 batch의 lightQueue에 nullptr를 추가한다.
					}

					bool allowInstancing = info.bAllowInstancing;
					// 추가 설정 판단 구문이 더 있다.

					addBatchToQueue(*info.pBatchQueue, batch, pTech, allowInstancing);
				}
			}
		}
	}

	/*
		1. LightBatchQueue는 Light 하나에 BatchQueue가 매칭된다.
		ForwardLight일 경우엔 draw call이 동일하지만 deferred는 좀 더 생각해봐야 한다.
		2. LightBatchQueue에서 BatchQueue를 어디에서 가져올지 고민해봐야 한다.
		3. StaticBatch 역시 LightBatchQueue를 가진다. 역시 어디에서 어떻게 연결되는지 분석이 필요하다.
		=> 복잡한 구현을 한 번에 따라갈 필요는 없다.
	*/
	void View::getLightBatches()
	{
		if (m_Lights.empty())
			return;

		// 특정 pass의 BatchQueue를 사용하는데... 그게 뭔지 모르겠다.
		//BatchQueue* pAlphaQueue = 

		// 결국 forwardlight는 light * mesh로 draw call이 발생한다.
		m_LightBatchQueues.resize(m_Lights.size());

		size_t queueIndex = 0;
		for (auto* pLight : m_Lights)
		{
			LightBatchQueue& lightQueue = m_LightBatchQueues[queueIndex++];
			lightQueue.pLight = pLight;
			lightQueue.BaseLitBatches.Clear();
		}

		// GetLitBatches()라는 곳에서 다시 AddBatchToQueue()를 이용해 최종적으로 lightQueue의 BaseLitBatches를 구성한다.
		// GetLitBatches()는 일단 Drawable 중에서 Light의 영향을 받는 것들을 추려놓는 함수인듯 하다. 
		// 그런데 alphaQueue는 왜 전달하지?
	}

	// light, geometry queue가 다르기 때문에 인자로 받는다.
	// 일단 allowShadows는 뺐다.
	void View::addBatchToQueue(BatchQueue& queue, StaticBatch& batch, Technique* pTech, bool allowInstancing)
	{
		if (!batch.GetMaterial())
			batch.SetMaterial(m_pRenderer->GetDefaultMaterial());
	
		// batch의 eGeomtryType이 static이라도 allowInstancing이고 indexbuffer가 존재한다면 instancing으로 바꾼다.
		// 일단 static만 구현하자.
		//batch.SetGeometryType(eGeometryType::Static);
		if (allowInstancing && batch.GetGeometryType() == eGeometryType::Static && batch.GetMesh()->GetIndexBuffer())
			batch.SetGeometryType(eGeometryType::Instanced);

		if (batch.GetGeometryType() == eGeometryType::Instanced)
		{
			// 해당 batch로 hash key를 만든다.

			// 전달받은 queue의 batchGroup에 해당 키를 가진 group이 있나 확인한다.
			// 없는 경우 일단 group을 생성하고 queue에 추가한다.
			{
				// geometry type은 일단 static으로 한다. 이유는 단일 사용일 수 있기 때문? 
			}

			// 추가 처리
		}
		else
		{
			// static인 경우 queue의 batches에 추가한다.
			// 다만 이때 worldTransform은 하나 혹은 다수일 어떤 처리를 하는데 아직 모르겠다.
			m_pRenderer->SetBatchShaders(batch, *pTech, false, queue);
			queue.AddStaticBatch(batch);
		}
	}

	void View::executeRenderPathCommands()
	{
		for (uint32_t i = 0; i < m_pRenderPath->GetCommandCount(); ++i)
		{
			RenderPathCommand* pCommand = m_pRenderPath->GetCommand(i);

			switch (pCommand->Type)
			{
			case eRenderCommandType::Clear:
			{
				setRenderTargets(pCommand);
				// 첫 번째 인자 역시 command의 clear flags이다.
				// 그런데 정의가 좀 생소하다.
				m_pGraphics->Clear(0, pCommand->ClearColor, pCommand->ClearDepth, pCommand->ClearStencil);
			}
			break;

			case eRenderCommandType::ScenePass:
			{
				BatchQueue& queue = m_BatchQueues[pCommand->PassIndex];
				if (!queue.IsEmpty())
				{
					setRenderTargets(pCommand);
					setTextures(pCommand);

					queue.Draw(this, m_pCamera);
				}
			}
			break;

			case eRenderCommandType::ForwardLights:
			{
				if (!m_LightBatchQueues.empty())
				{
					setRenderTargets(pCommand);

					for (auto it = m_LightBatchQueues.begin(); it != m_LightBatchQueues.end(); ++it)
					{
						it->BaseLitBatches.Draw(this, m_pCamera);
					}
				}
			}
			break;

			default:
				return;
			}
		}
	}

	// command로부터 renderTarget, DepthStencil 그리고 Viewport를 참조하여 바인딩한다.
	void View::setRenderTargets(RenderPathCommand* pCommand)
	{
		if (!pCommand)
			return;

		uint32_t index = 0;

		while(index < static_cast<uint32_t>(pCommand->Outputs.size()))
		{
			if(pCommand->Outputs[index].first == "viewport")
			{
				m_pGraphics->SetRenderTarget(0, dynamic_cast<RenderTexture*>(m_pCurRenderTarget));
				// useViewportOutput = true;
			}
			else
			{
				// 일단 outputs에서 이름을 이용해 FindNamedTexture()에서 텍스쳐를 가져온다.

				// 중간에 if문으로 구분을 하는데
				// 결국 Graphics의 SetRenderTarget에 index와 텍스쳐를 전달한다.
				// => SetRenderTarget는 Graphics의 m_RenderTargets 배열에 저장만 하기 때문에 가능하다.
				// => 이후 prepareDraw에서 OMSetRenderTargets()로 전달한다.
			}

			++index;
		}

		while (index < 4)
		{
			m_pGraphics->SetRenderTarget(index, nullptr);
			++index;
		}

		// depth stencil
		// command의 depthstencilName이 존재할 경우
		{
			// 역시 FindNAmedTexture()에서 이름을 이용해 찾아온 후 
			// Graphics의 SetDepthStencil에 전달한다.
		}

		// 여기에서 크기 설정이 추가된다. ViewRect은 이를 이용해 구성된다.
		m_pGraphics->SetViewportRect(m_ViewRect);
		// 위의 depthStencil이 존재하지 않을 때
		// 다른 depthStencil을 가져완 SetDepthStencil을 한다. 이는 좀 더 찾아봐야 할 듯...
	}

	// 커맨드로부터 이름을 획득해 텍스쳐를 바인딩한다.
	void View::setTextures(RenderPathCommand* pCommand)
	{
		if (!pCommand)
			return;

		for (uint32_t i = 0; i < 16; ++i)
		{
			if (pCommand->TextureNames[i].empty())
				continue;

			// 일종의 환경맵핑 같은데 왜 continue인지는 모르겠다.
			if (pCommand->TextureNames[i] == "viewport")
			{
				// 텍스쳐 클래스 변경 중...
				//m_pGraphics->SetTexture(i, m_pCurRenderTarget);
				continue;
			}

			// 마지막으로 FindNamedTexture로부터 찾아와 SetTexture한다.
			Texture* pTexture = findNamedTexture(pCommand->TextureNames[i], false);
			if (pTexture)
			{
				// 텍스쳐 클래스 변경 중...
				//m_pGraphics->SetTexture(i, pTexture);
			}
			else
			{
				// 존재하지 않는다면 커맨드를 수정한다.
				// 그렇데 빈 값을 넣는게 이게 맞나?
				pCommand->TextureNames[i] = std::string();
			}
		}
	}

	// 일단 이름은 경로를 포함토록 한다.
	Texture* View::findNamedTexture(const std::string& name, bool bRenderTarget)
	{
		const std::string onlyName = FileSystem::GetFileName(name);
		
		ResourceCache* pCache = GetSubsystem<ResourceCache>();
		Texture* pTexture = pCache->GetExistingResource<Texture2D>(onlyName);
		if (pTexture)
			return pTexture;
		// 존재하지 않는다면 cube, 3d, 2d array도 시도한다.
		
		// 이게 의미하는바를 모르겠다.
		if (!bRenderTarget)
		{
			// 우선 xml을 파싱해서 캐시로부터 찾거나 생성한다. 아직 잘 모르겠다.

			// 아니면 전달받은 경로를 통해 캐시로부터 찾거나 생성한다.
			return pCache->GetResource<Texture2D>(name);
		}

		return nullptr;
	}
}