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
		// �����δ� octree�� cullCamera �̴�.
		if (!m_pScene)
			return;

		getDrawables();
		getBaseBatches();
	}
	
	void View::Render()
	{
		// UpdateGeometries

		// ���� RenderPath�� �ϵ��ڵ� �س��� �����̴�.
		// Clear
		{
			m_pCurrentRenderTarget = m_pRenderTarget;

			//eClearTarget flags = eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil;
			m_pGraphics->SetRenderTarget(0, dynamic_cast<Texture2D*>(m_pCurrentRenderTarget));
			m_pGraphics->Clear(0, DirectX::XMFLOAT4(1.0f, 1.0f, 0.6f, 1.0f), 1.0f, 0);

			// �̰� �ӽ� ��ġ
			m_pGraphics->SetViewportRect(m_ViewRect);
		}

		// ScenePass
		{
			auto pDeviceContext = m_pGraphics->GetDeviceContext();
			
			for (auto batchRenderer : m_BaseBatchRenderers)
			{
				batchRenderer.Draw(this);
			}

			// ��ġ�� �ָ��ϴ�. ������ clear�ϴ� ���� �´�.
			m_BaseBatchRenderers.clear();
		}
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderTarget = pRenderTarget;

		// pViewport�κ��� RenderPath�� ȹ��
		m_pScene = pViewport->GetScene();
		// ī�޶� ȹ��

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

		// ���� �ʱ�ȭ
		m_ScenePasses.clear();

		// ScenePasses��� ���� �����Ѵ�.
		// �̴� RenderPathCommand�� Technique�� Pass�� �����ϴ� ���� ����.
		{

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


		m_Drawables.clear();
		auto allGameObjects = m_pScene->GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			// �̷��� SkinnedDrawable�� �ȵ��ٵ�...
			// �̷��� ���� ��ġ����.
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
				
				// �����δ� ScenePasses�� ������ ���� Batch�� ������ �� AddBatchToQueue()���� �����Ѵ�.
				// ��, Batches�� Pass�� ���� batch�� �����ϴ� ����ü�̴�.

				// AddBatchToQueue()�� �ᱹ static�� instance�� �����Ͽ� �����ϴ� ó���̴�.
				// �̸� �̿��� batch rendering�� �ƴ� insctancing rendering�� �����ϰ� �ȴ�.

				auto* pTech = drawableBatch.m_pMaterial->GetTechnique();
				if (!pTech)
					continue;

				// ScenePasses ����
				{
					// pTech�κ��� pass�� �����;��ϴµ� �ε����� �ʿ��ϴ�.
					// urho�� ScenePassInfo�� �ε����� �ִ�.
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

							// graphics�� Ÿ��, �̸�, defines�� ������ ���´�.
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