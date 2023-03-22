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
		DV_LOG_ENGINE_TRACE("View �Ҹ� �Ϸ�");
	}

	void View::Update(float delta)
	{
		// �����δ� octree�� cullCamera �̴�.
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
		// ���⿡�� queue�� sort�� �Ѵ�.

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

		// ���� �ʱ�ȭ
		m_ScenePasses.clear();

		// ScenePass Command�� �Ϻ������� m_ScenePasses ����
		for(uint32_t i = 0; i < m_pRenderPath->GetCommandCount(); ++i)
		{
			RenderPathCommand* pCommand = m_pRenderPath->GetCommand(i);
			// unabled Ȯ��

			// �ᱹ �ٸ� �н������� m_ScenePasses�� m_BatchQueues�� �������� �ʴ´�.
			// ����Ʈ�� ��� m_LightQueues�� ����ϴ� ���� ������ ���еǴ� �� ����.
			if (pCommand->Type == eRenderCommandType::ScenePass)
			{
				ScenePassInfo info;
				info.PassIndex = pCommand->PassIndex = Technique::GetPassIndex(pCommand->Pass);
				info.bAllowInstancing = false;	// command.sortMode != SORT_BACKTOFRONT
				info.bMarkStencil;
				info.bVertexLight = pCommand->bVertexLights;

				// ���� Pass�� m_BatchQueues�� m_ScenePassed�� BatchQueue�� ����
				auto it = m_BatchQueues.find(info.PassIndex);
				if (it == m_BatchQueues.end())
					m_BatchQueues.emplace(info.PassIndex, BatchQueue());
				info.pBatchQueue = &m_BatchQueues[info.PassIndex];

				m_ScenePasses.emplace_back(info);
			}
		}

		// �̿ܿ��� ���� ó���� �����Ѵ�.

		return true;
	}

	void View::getDrawables()
	{
		// �ϴ� �׷����� �� ������Ʈ���� �߸���.
		// ��Ʈ���� �ø��� ��ģ ������Ʈ�� �� �ϴ�.
		// �׷��� Light�� Drawable�ϱ�?
		// ������ Light�� Drwable�� ����ߴ�... ���� ������ �ʿ�� ���� �ʳ�?
		// => ������ �� ������ �з��ϴ� ���� �� �����ϴ�.


		auto allGameObjects = m_pScene->GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			// �̷��� SkinnedDrawable�� �ȵ��ٵ�...
			// �̷��� ���� ��ġ����.
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
				
				// ���⿡�� Material�� ���ٸ� ������ �߻��Ѵ�.
				// urho�� GetTechnique�̶�� �Լ��� ����� drawable�� material�� �����Ͽ� technique�� ���Ϲ޵��� �ߴ�.
				Technique* pTech = drawableBatch.pMaterial->GetTechnique();
				if (!pTech)
					continue;

				// �̰� BaseBatche�� Ư�����̴�.
				// Pass���� BatchQueue�� �����ϰ� �ִ�.
				// ��, �ϳ��� Mesh�� Pass�� ���� �پ��� ������ Shading�� ������ �� �ִٴ� ���̴�.
				for(uint32_t j = 0; j < static_cast<uint32_t>(m_ScenePasses.size()); ++j)
				{
					ScenePassInfo& info = m_ScenePasses[j];

					// base index��� �� skip�Ѵ�.. ���� ���� �Ұ�.

					Pass* pPass = pTech->GetPass(info.PassIndex);
					if (!pPass)
						continue;

					StaticBatch batch(drawableBatch);
					batch.SetPass(pPass);

					// vertexLight�� vertexColor�� �ٸ���. ȥ���� �� �ϴ�.
					if (info.bVertexLight)
					{
						// light�� ã�� ����Ѵ�. �ڼ����� �𸣰ڴ�.
					}
					else
					{
						// ���� batch�� lightQueue�� nullptr�� �߰��Ѵ�.
					}

					bool allowInstancing = info.bAllowInstancing;
					// �߰� ���� �Ǵ� ������ �� �ִ�.

					addBatchToQueue(*info.pBatchQueue, batch, pTech, allowInstancing);
				}
			}
		}
	}

	/*
		1. LightBatchQueue�� Light �ϳ��� BatchQueue�� ��Ī�ȴ�.
		ForwardLight�� ��쿣 draw call�� ���������� deferred�� �� �� �����غ��� �Ѵ�.
		2. LightBatchQueue���� BatchQueue�� ��𿡼� �������� ����غ��� �Ѵ�.
		3. StaticBatch ���� LightBatchQueue�� ������. ���� ��𿡼� ��� ����Ǵ��� �м��� �ʿ��ϴ�.
		=> ������ ������ �� ���� ���� �ʿ�� ����.
	*/
	void View::getLightBatches()
	{
		if (m_Lights.empty())
			return;

		// Ư�� pass�� BatchQueue�� ����ϴµ�... �װ� ���� �𸣰ڴ�.
		//BatchQueue* pAlphaQueue = 

		// �ᱹ forwardlight�� light * mesh�� draw call�� �߻��Ѵ�.
		m_LightBatchQueues.resize(m_Lights.size());

		size_t queueIndex = 0;
		for (auto* pLight : m_Lights)
		{
			LightBatchQueue& lightQueue = m_LightBatchQueues[queueIndex++];
			lightQueue.pLight = pLight;
			lightQueue.BaseLitBatches.Clear();
		}

		// GetLitBatches()��� ������ �ٽ� AddBatchToQueue()�� �̿��� ���������� lightQueue�� BaseLitBatches�� �����Ѵ�.
		// GetLitBatches()�� �ϴ� Drawable �߿��� Light�� ������ �޴� �͵��� �߷����� �Լ��ε� �ϴ�. 
		// �׷��� alphaQueue�� �� ��������?
	}

	// light, geometry queue�� �ٸ��� ������ ���ڷ� �޴´�.
	// �ϴ� allowShadows�� ����.
	void View::addBatchToQueue(BatchQueue& queue, StaticBatch& batch, Technique* pTech, bool allowInstancing)
	{
		if (!batch.GetMaterial())
			batch.SetMaterial(m_pRenderer->GetDefaultMaterial());
	
		// batch�� eGeomtryType�� static�̶� allowInstancing�̰� indexbuffer�� �����Ѵٸ� instancing���� �ٲ۴�.
		// �ϴ� static�� ��������.
		//batch.SetGeometryType(eGeometryType::Static);
		if (allowInstancing && batch.GetGeometryType() == eGeometryType::Static && batch.GetMesh()->GetIndexBuffer())
			batch.SetGeometryType(eGeometryType::Instanced);

		if (batch.GetGeometryType() == eGeometryType::Instanced)
		{
			// �ش� batch�� hash key�� �����.

			// ���޹��� queue�� batchGroup�� �ش� Ű�� ���� group�� �ֳ� Ȯ���Ѵ�.
			// ���� ��� �ϴ� group�� �����ϰ� queue�� �߰��Ѵ�.
			{
				// geometry type�� �ϴ� static���� �Ѵ�. ������ ���� ����� �� �ֱ� ����? 
			}

			// �߰� ó��
		}
		else
		{
			// static�� ��� queue�� batches�� �߰��Ѵ�.
			// �ٸ� �̶� worldTransform�� �ϳ� Ȥ�� �ټ��� � ó���� �ϴµ� ���� �𸣰ڴ�.
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
				// ù ��° ���� ���� command�� clear flags�̴�.
				// �׷��� ���ǰ� �� �����ϴ�.
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

	// command�κ��� renderTarget, DepthStencil �׸��� Viewport�� �����Ͽ� ���ε��Ѵ�.
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
				// �ϴ� outputs���� �̸��� �̿��� FindNamedTexture()���� �ؽ��ĸ� �����´�.

				// �߰��� if������ ������ �ϴµ�
				// �ᱹ Graphics�� SetRenderTarget�� index�� �ؽ��ĸ� �����Ѵ�.
				// => SetRenderTarget�� Graphics�� m_RenderTargets �迭�� ���常 �ϱ� ������ �����ϴ�.
				// => ���� prepareDraw���� OMSetRenderTargets()�� �����Ѵ�.
			}

			++index;
		}

		while (index < 4)
		{
			m_pGraphics->SetRenderTarget(index, nullptr);
			++index;
		}

		// depth stencil
		// command�� depthstencilName�� ������ ���
		{
			// ���� FindNAmedTexture()���� �̸��� �̿��� ã�ƿ� �� 
			// Graphics�� SetDepthStencil�� �����Ѵ�.
		}

		// ���⿡�� ũ�� ������ �߰��ȴ�. ViewRect�� �̸� �̿��� �����ȴ�.
		m_pGraphics->SetViewportRect(m_ViewRect);
		// ���� depthStencil�� �������� ���� ��
		// �ٸ� depthStencil�� ������ SetDepthStencil�� �Ѵ�. �̴� �� �� ã�ƺ��� �� ��...
	}

	// Ŀ�ǵ�κ��� �̸��� ȹ���� �ؽ��ĸ� ���ε��Ѵ�.
	void View::setTextures(RenderPathCommand* pCommand)
	{
		if (!pCommand)
			return;

		for (uint32_t i = 0; i < 16; ++i)
		{
			if (pCommand->TextureNames[i].empty())
				continue;

			// ������ ȯ����� ������ �� continue������ �𸣰ڴ�.
			if (pCommand->TextureNames[i] == "viewport")
			{
				// �ؽ��� Ŭ���� ���� ��...
				//m_pGraphics->SetTexture(i, m_pCurRenderTarget);
				continue;
			}

			// ���������� FindNamedTexture�κ��� ã�ƿ� SetTexture�Ѵ�.
			Texture* pTexture = findNamedTexture(pCommand->TextureNames[i], false);
			if (pTexture)
			{
				// �ؽ��� Ŭ���� ���� ��...
				//m_pGraphics->SetTexture(i, pTexture);
			}
			else
			{
				// �������� �ʴ´ٸ� Ŀ�ǵ带 �����Ѵ�.
				// �׷��� �� ���� �ִ°� �̰� �³�?
				pCommand->TextureNames[i] = std::string();
			}
		}
	}

	// �ϴ� �̸��� ��θ� ������� �Ѵ�.
	Texture* View::findNamedTexture(const std::string& name, bool bRenderTarget)
	{
		const std::string onlyName = FileSystem::GetFileName(name);
		
		ResourceCache* pCache = GetSubsystem<ResourceCache>();
		Texture* pTexture = pCache->GetExistingResource<Texture2D>(onlyName);
		if (pTexture)
			return pTexture;
		// �������� �ʴ´ٸ� cube, 3d, 2d array�� �õ��Ѵ�.
		
		// �̰� �ǹ��ϴ¹ٸ� �𸣰ڴ�.
		if (!bRenderTarget)
		{
			// �켱 xml�� �Ľ��ؼ� ĳ�÷κ��� ã�ų� �����Ѵ�. ���� �� �𸣰ڴ�.

			// �ƴϸ� ���޹��� ��θ� ���� ĳ�÷κ��� ã�ų� �����Ѵ�.
			return pCache->GetResource<Texture2D>(name);
		}

		return nullptr;
	}
}