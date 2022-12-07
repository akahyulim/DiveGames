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
#include "Resource/ResourceCache.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Drawable.h"
#include "IO/Log.h"
#include "Math/Math.h"

namespace Dive 
{
	View::View(Context* pContext)
		: Object(pContext)
	{
		m_pGraphics = GetSubsystem<Graphics>();
		m_pRenderer = GetSubsystem<Renderer>();
	}

	View::~View()
	{
		DV_LOG_ENGINE_DEBUG("Destroy View");
	}

	void View::Update(float delta)
	{
		// 실제로는 octree와 cullCamera 이다.
		if (!m_pScene)
			return;

		getDrawables();
		getBaseBatches();
	}
	
	void View::Render()
	{
		// UpdateGeometries

		// 현재 RenderPath를 하드코딩 해놓은 상태이다.
		// Clear
		{
			m_pCurrentRenderTarget = m_pRenderTarget;

			//eClearTarget flags = eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil;
			m_pGraphics->SetRenderTarget(0, dynamic_cast<Texture2D*>(m_pCurrentRenderTarget));
			m_pGraphics->Clear(0, DirectX::XMFLOAT4(1.0f, 1.0f, 0.6f, 1.0f), 1.0f, 0);

			// 이건 임시 위치
			m_pGraphics->SetViewportRect(m_ViewRect);
		}

		// ScenePass
		{
			auto pDeviceContext = m_pGraphics->GetDeviceContext();
			
			for (auto batchRenderer : m_BaseBatchRenderers)
			{
				batchRenderer.Draw(this);
			}

			// 위치가 애매하다. 하지만 clear하는 것이 맞다.
			m_BaseBatchRenderers.clear();
		}
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderTarget = pRenderTarget;

		// pViewport로부터 RenderPath를 획득
		m_pScene = pViewport->GetScene();
		// 카메라도 획득

		int width = pRenderTarget ? pRenderTarget->GetWidth() : m_pGraphics->GetWidth();
		int height = pRenderTarget ? pRenderTarget->GetHeight() : m_pGraphics->GetHeight();
		m_RenderTargetSize = DirectX::XMINT2(width, height);

		auto rect = pViewport->GetRect();
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

		// ScenePasses라는 것을 구성한다.
		// 이는 RenderPathCommand와 Technique의 Pass를 연결하는 과정 같다.
		{

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


		m_Drawables.clear();
		auto allGameObjects = m_pScene->GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			// 이러면 SkinnedDrawable은 안될텐데...
			// 미래의 내가 고치겠지.
			if (pGameObject->HasComponent<Dive::Drawable>())
				m_Drawables.emplace_back(pGameObject->GetComponent<Dive::Drawable>());
		}
	}

	void View::getBaseBatches()
	{
		for (auto it = m_Drawables.begin(); it != m_Drawables.end(); ++it)
		{
			auto pDrawable = *it;

			const auto& sourceDatas = pDrawable->GetSourceDatas();

			for (unsigned i = 0; i < static_cast<unsigned int>(sourceDatas.size()); ++i)
			{
				const auto& drawableBatch = sourceDatas[i];
				
				// 실제로는 ScenePasses에 루프를 돌려 Batch를 생성한 후 AddBatchToQueue()에서 저장한다.
				// 즉, Batches는 Pass에 맞춰 batch를 관리하는 구조체이다.

				// AddBatchToQueue()는 결국 static과 instance를 구분하여 저장하는 처리이다.
				// 이를 이용해 batch rendering이 아닌 insctancing rendering이 가능하게 된다.

				auto* pTech = drawableBatch.m_pMaterial->GetTechnique();
				if (!pTech)
					continue;

				// ScenePasses 루프
				{
					// pTech로부터 pass를 가져와야하는데 인덱스가 필요하다.
					// urho는 ScenePassInfo에 인덱스가 있다.
					auto* pPass = pTech->GetPass(0);

					Batch batch(drawableBatch);
					batch.m_pPass = pPass;
					
					// Renderer::SetBatchShaders
					{
						auto vertexShaderVariations = pPass->GetVertexShaderVariations();
						auto pixelShaderVariations = pPass->GetPixelShaderVariations();

						if(vertexShaderVariations.empty() || pixelShaderVariations.empty())
						{
							// Renderer::LoadPassShaders
							vertexShaderVariations.clear();
							pixelShaderVariations.clear();

							// graphics에 타입, 이름, defines를 전달해 얻어온다.
							vertexShaderVariations.emplace_back(
								m_pGraphics->GetShader(eShaderType::Vertex, pPass->GetVertexShaderName(), pPass->GetVertexShaderDefines()));

							pixelShaderVariations.emplace_back(
								m_pGraphics->GetShader(eShaderType::Pixel, pPass->GetPixelShaderName(), pPass->GetPixelShaderDefines()));
						}

						batch.m_pVertexShader = vertexShaderVariations[0];
						batch.m_pPixelShader = pixelShaderVariations[0];
					}

					m_BaseBatchRenderers.emplace_back(batch);
				}
			}
		}
	}
}