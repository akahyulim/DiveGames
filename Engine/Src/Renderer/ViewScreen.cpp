#include "divepch.h"
#include "ViewScreen.h"
#include "Material.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/CoreDefs.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Renderable.h"
#include "Scene/Components/Light.h"


namespace Dive
{
	ViewScreen::ViewScreen(Camera* pCamera, eRenderPath renderPath)
		: m_pCamera(pCamera)
		, m_RenderPath(renderPath)
	{
		m_pActiveScene = SceneManager::GetActiveScene();

		// �ӽ�: create constant buffer 
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer,sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	}

	// ���ĸ�ź�� �̺�Ʈ �ø� �Լ����� �����Ѵ�.
	void ViewScreen::Update()
	{
		if (!m_pActiveScene || m_pActiveScene->IsEmpty())
			return;

		for (auto it = m_Renderables.begin(); it != m_Renderables.end(); ++it)
		{
			it->second.clear();
		}

		// �ӽ�
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer, sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));

		for (auto pGameObject : m_pActiveScene->GetAllGameObjects())
		{
			if (!pGameObject->IsActive())
				continue;

			if (auto pRenderableCom = pGameObject->GetComponent<Renderable>())
			{
				if (auto pMaterial = pRenderableCom->GetMaterial())
				{
					m_Renderables[pMaterial->IsOpaque() ? eRenderable::Opaque : eRenderable::Transparent].emplace_back(pGameObject);
				}
			}

			if (auto pLightCom = pGameObject->GetComponent<Light>())
			{
				m_Renderables[eRenderable::Light].emplace_back(pGameObject);
			}
		}

		// ������ ������Ʈ�� ī�޶󿡼� �� �� ���� �׷��� �ϹǷ� ������ �ʿ��ϴٰ� �Ѵ�.
		// ���ĸ�ź���� �ش� �κ��� ���� ã�� ���ߴ�.
	}
	
	void ViewScreen::Render()
	{
		forwardRender();
	}

	void ViewScreen::forwardRender()
	{
		// frustum ����
		{
			// ������ ���ڴ� ī�޶󿡼� �� �� �ִ� �ִ� �Ÿ��� ���ϴ� �� �ϴ�.
			// �׷��� spartan�� �ٰŸ��� �����س��Ҵ�.
			m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());
		}

		// set & clear rendertarget
		{
			Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
			Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);
		}

		// urho ������ �� �κе� Batch::Prepare()�� ���Ѵ�.
		// ��, Batch���� ȣ��ǹǷ� �ߺ� üũ�� Ȯ���ϴ� ���� �¾ƺ��δ�.
		{
			// ����Ʈ
			// �ϴ� ���ܽ������� ī�޶� Ư�� �κ��� ���� �� �־�� �Ѵ�.
			// => �̰� �ᱹ ViewRect�ƴѰ�? ���� ���������Ѵ�.
			// => �׷��� ���ʿ� urho���� �̸��� Viewport��.
			//Graphics::SetViewport(m_pCamera->GetViewport());

			{
				// ���۸� Renderer�� �����Ѵٸ� cpu���۴� �� ���� �ƴ϶� GraphcisDevice�� �����ϴ� ���� �´�.
				// �ٸ� �� ������ Graphics�� �����ϱ� ���ؼ�
				// 1. ������ ����, 2. ������ ������, 3. offset�� �ʿ��� ���δ�.
				// urho�� ��� SetShaderParamter��� �Լ��� �̸� �����ϴµ�
				// ���� �̸��� Ÿ�Ժ� �����͸� �����ϸ�
				// ���ο��� �ش� �̸��� ���۸� ������ �����͸� ����ְ� ��Ƽüũ�� �ϴ� �� �ϴ�.
				// �� ��� �����ͺ� offset�� �߿�������.
				m_cpuFrameBuffer.view = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
				m_cpuFrameBuffer.projection = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			}

			{
				m_cpuCameraBuffer.position = m_pCamera->GetPosition();

				// �ָ������� ��ο��ݸ��� �� ���� �ص� �Ǵ� �۾��̴�.
				auto pCameraBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}

			// states
			// ������ Renderer���� �� �س��� ����
			// �ᱹ RenderPath�� ���� � State�� ��� ������ ����ϴ��ĸ� �ľ��ؾ� �Ѵ�.
			// �׸��� urho�� ���� �ʿ� ���� �ϴ� �ϵ� �ڵ����� ��� state�� �����ϴ� ���� �¾� ���δ�.
			{
				Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
			}
		}

		// bind lights
		// forward light�� light x mesh
		{
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightComponent = pLight->GetComponent<Light>();

				switch (pLightComponent->GetType())
				{
				case eLightType::Directional:
					m_cpuLightBuffer.options = (1U << 0);
					m_cpuLightBuffer.direction = pLight->GetForward();
					break;
				case eLightType::Point:
					m_cpuLightBuffer.options = (1U << 1);
					m_cpuLightBuffer.position = pLight->GetPosition();
					m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
					break;
				case eLightType::Spot:
					m_cpuLightBuffer.options = (1U << 2);
					m_cpuLightBuffer.position = pLight->GetPosition();
					m_cpuLightBuffer.direction = pLight->GetForward();
					m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
					m_cpuLightBuffer.outerConeAngle = pLightComponent->GetOuterAngleRadian();
					m_cpuLightBuffer.innerConeAngle = pLightComponent->GetInnerAngleRadian();
					break;
				default:
					break;
				}

				m_cpuLightBuffer.color = { pLightComponent->GetColor().x * pLightComponent->GetColor().x
					, pLightComponent->GetColor().y * pLightComponent->GetColor().y
					, pLightComponent->GetColor().z * pLightComponent->GetColor().z };

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				if (i == 0)
					Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
				else if (i == 1)
					Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Additive));

				// draw opaques
				for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
				{
					auto* pRenderableComponent = pOpaque->GetComponent<Renderable>();
					const auto* pMaterial = pRenderableComponent->GetMaterial();
					const auto& boundingBox = pRenderableComponent->GetBoundingBox();

					if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
						continue;

					m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pOpaque->GetMatrix());

					auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
					pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
					pFrameBuffer->Bind();	// ���̴� Ÿ�Ժ� ���ε� �� ���� ���������� �� ����� �� ���ϴ�...

					auto pShader = pMaterial->GetShader();
					Graphics::SetShader(pShader);

					m_cpuMaterialBuffer.diffuseColor = pMaterial->GetDiffuseColor();
					m_cpuMaterialBuffer.properties = 0;
					m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;
					m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Normal) ? (1U << 1) : 0;

					auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
					pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
					pMaterialBuffer->Bind();

					Graphics::SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
					Graphics::SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));

					// ��� ���� ���뵵 ���⿡�� �� ó���� �� �ִµ�...
					pRenderableComponent->Draw();
				}
			}

			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightComponent = pLight->GetComponent<Light>();

				switch (pLightComponent->GetType())
				{
				case eLightType::Directional:
					m_cpuLightBuffer.options = (1U << 0);
					m_cpuLightBuffer.direction = pLight->GetForward();
					break;
				case eLightType::Point:
					m_cpuLightBuffer.options = (1U << 1);
					m_cpuLightBuffer.position = pLight->GetPosition();
					m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
					break;
				case eLightType::Spot:
					m_cpuLightBuffer.options = (1U << 2);
					m_cpuLightBuffer.position = pLight->GetPosition();
					m_cpuLightBuffer.direction = pLight->GetForward();
					m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
					m_cpuLightBuffer.outerConeAngle = pLightComponent->GetOuterAngleRadian();
					m_cpuLightBuffer.innerConeAngle = pLightComponent->GetInnerAngleRadian();
					break;
				default:
					break;
				}

				m_cpuLightBuffer.color = { pLightComponent->GetColor().x * pLightComponent->GetColor().x
					, pLightComponent->GetColor().y * pLightComponent->GetColor().y
					, pLightComponent->GetColor().z * pLightComponent->GetColor().z };

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				Graphics::SetDepthStencilState(NULL);
				Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Transparent));

				// draw transparents
				for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
				{
					auto* pRenderableComponent = pTransparent->GetComponent<Renderable>();
					const auto* pMaterial = pRenderableComponent->GetMaterial();
					const auto& boundingBox = pRenderableComponent->GetBoundingBox();

					if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
						continue;

					m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pTransparent->GetMatrix());

					auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
					pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
					pFrameBuffer->Bind();	// ���̴� Ÿ�Ժ� ���ε� �� ���� ���������� �� ����� �� ���ϴ�...

					auto pShader = pMaterial->GetShader();
					Graphics::SetShader(pShader);

					m_cpuMaterialBuffer.diffuseColor = pMaterial->GetDiffuseColor();
					m_cpuMaterialBuffer.properties = 0;
					m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;
					m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Normal) ? (1U << 1) : 0;

					auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
					pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
					pMaterialBuffer->Bind();

					Graphics::SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
					Graphics::SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));

					// ��� ���� ���뵵 ���⿡�� �� ó���� �� �ִµ�...
					pRenderableComponent->Draw();
				}
			}

			Graphics::SetDepthStencilState(NULL);
			Graphics::SetBlendState(NULL);
		}
	}

	void ViewScreen::deferredRender()
	{
		// set & clear g buffer

		// prepare draw(bind state, viewport)

		// draw opaque(& bind constant buffer, shader, buffer) 

		// set & clear rendertarget

		// prepare draw

		// draw light
	}
}