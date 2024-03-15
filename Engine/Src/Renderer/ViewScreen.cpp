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
#include "Scene/Components/Transform.h"
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

	void ViewScreen::Update()
	{
		if (!m_pActiveScene || m_pActiveScene->IsEmpty())
			return;

		m_Renderables.clear();
		m_Lights.clear();

		// �ӽ�
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer, sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));

		for (auto pGameObject : m_pActiveScene->GetAllGameObjects())
		{
			auto pRenderableCom = pGameObject->GetComponent<Renderable>();
			if (pRenderableCom)
			{
				m_Renderables.push_back(pRenderableCom);
				continue;
			}

			auto pLightCom = pGameObject->GetComponent<Light>();
			if (pLightCom)
			{
				m_Lights.push_back(pLightCom);
				continue;
			}
		}	
	}
	
	void ViewScreen::Render()
	{
		forwardRender();
	}

	void ViewScreen::forwardRender()
	{
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
				// �����غ��� ����� ���⿡�� �����ص� �ȴ�.
				//Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
				//Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));
			}
		}

		// bind lights
		// forward light�� light x mesh
		{
			for (int i = 0; i < (int)m_Lights.size(); i++)
			{
				auto light = m_Lights[i];
				if (!light->IsEnabled())
					continue;

				switch (light->GetType())
				{
				case eLightType::Directional:
					m_cpuLightBuffer.options = (1U << 0);
					m_cpuLightBuffer.dirToLight = { -light->GetDirection().x, -light->GetDirection().y, -light->GetDirection().z };
					break;
				case eLightType::Point:
					m_cpuLightBuffer.options = (1U << 1);
					m_cpuLightBuffer.position = light->GetTransform()->GetPosition();
					m_cpuLightBuffer.rangeRcp = 1.0f / light->GetRange();
					break;
				case eLightType::Spot:
					m_cpuLightBuffer.options = (1U << 2);
					m_cpuLightBuffer.position = light->GetTransform()->GetPosition();
					m_cpuLightBuffer.dirToLight = { -light->GetDirection().x, -light->GetDirection().y, -light->GetDirection().z };
					m_cpuLightBuffer.rangeRcp = 1.0f / light->GetRange();
					m_cpuLightBuffer.cosOuterCone = cosf(light->GetOuterAngleRadian());
					m_cpuLightBuffer.cosInnerConeRcp = 1.0f / cosf(light->GetInnerAngleRadian());
					break;
				default:
					break;
				}

				m_cpuLightBuffer.color = light->GetColor();

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				// 1. ���� �����ϴ�.
				// eState�� �����ϸ� Graphics�� �ش� �Լ� ���ο��� Renderer�� ���� �����ص� ���� ������?
				// �ƴ�, �׳� Graphics���� �����ϰ� �����ϸ� �ȵǳ�..?
				// 2. �� �κж����� Light ���� ����� �̻�������.
				if (i == 0)
					Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
				else if (i >= 1)
					Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));

				// draw opaque
				{
					for (auto pRenderable : m_Renderables)
					{
						m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pRenderable->GetTransform()->GetWorldMatrix());

						auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
						pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
						pFrameBuffer->Bind();	// ���̴� Ÿ�Ժ� ���ε� �� ���� ���������� �� ����� �� ���ϴ�...

						auto pShader = pRenderable->GetMaterial()->GetShader();
						Graphics::SetShader(pShader);

						m_cpuMaterialBuffer.diffuseColor = pRenderable->GetMaterial()->GetDiffuseColor();
						m_cpuMaterialBuffer.properties = 0;
						m_cpuMaterialBuffer.properties |= pRenderable->GetMaterial()->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;

						auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
						pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
						pMaterialBuffer->Bind();

						Graphics::SetTexture(eTextureUnit::Diffuse, pRenderable->GetMaterial()->GetTexture(eTextureUnit::Diffuse));

						// ��� ���� ���뵵 ���⿡�� �� ó���� �� �ִµ�...
						pRenderable->Draw();
					}
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