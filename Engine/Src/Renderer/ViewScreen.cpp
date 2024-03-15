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

		// 임시: create constant buffer 
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

		// 임시
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

		// urho 구조상 이 부분도 Batch::Prepare()에 속한다.
		// 즉, Batch별로 호출되므로 중복 체크를 확인하는 편이 맞아보인다.
		{
			// 뷰포트
			// 일단 제외시켰지만 카메라가 특정 부분을 비출 수 있어야 한다.
			// => 이게 결국 ViewRect아닌가? 현재 오락가락한다.
			// => 그런데 애초에 urho에서 이름이 Viewport다.
			//Graphics::SetViewport(m_pCamera->GetViewport());

			{
				// 버퍼를 Renderer가 관리한다면 cpu버퍼는 이 곳이 아니라 GraphcisDevice가 관리하는 것이 맞다.
				// 다만 이 곳에서 Graphics에 전달하기 위해선
				// 1. 버퍼의 종류, 2. 적절한 데이터, 3. offset이 필요해 보인다.
				// urho의 경우 SetShaderParamter라는 함수가 이를 수행하는데
				// 버퍼 이름과 타입별 데이터를 전달하면
				// 내부에선 해당 이름의 버퍼를 가져와 데이터를 집어넣고 더티체크를 하는 듯 하다.
				// 이 경우 데이터별 offset이 중요해진다.
				m_cpuFrameBuffer.view = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
				m_cpuFrameBuffer.projection = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			}

			{
				m_cpuCameraBuffer.position = m_pCamera->GetPosition();

				// 애매하지만 드로우콜마다 한 번만 해도 되는 작업이다.
				auto pCameraBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}

			// states
			// 생성은 Renderer에서 다 해놓은 상태
			// 결국 RenderPath에 맞춰 어떤 State를 어느 시점에 사용하느냐를 파악해야 한다.
			// 그리고 urho를 따라갈 필요 없이 일단 하드 코딩으로 사용 state를 전달하는 것이 맞아 보인다.
			{
				Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
				// 생각해보니 현재는 여기에서 연결해도 된다.
				//Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
				//Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));
			}
		}

		// bind lights
		// forward light는 light x mesh
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

				// 1. 괜히 복잡하다.
				// eState를 전달하면 Graphics의 해당 함수 내부에서 Renderer를 통해 설정해도 되지 않을까?
				// 아니, 그냥 Graphics에서 생성하고 관리하면 안되나..?
				// 2. 이 부분때문에 Light 꺼도 결과가 이상해진다.
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
						pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

						auto pShader = pRenderable->GetMaterial()->GetShader();
						Graphics::SetShader(pShader);

						m_cpuMaterialBuffer.diffuseColor = pRenderable->GetMaterial()->GetDiffuseColor();
						m_cpuMaterialBuffer.properties = 0;
						m_cpuMaterialBuffer.properties |= pRenderable->GetMaterial()->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;

						auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
						pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
						pMaterialBuffer->Bind();

						Graphics::SetTexture(eTextureUnit::Diffuse, pRenderable->GetMaterial()->GetTexture(eTextureUnit::Diffuse));

						// 사실 위의 내용도 여기에서 다 처리할 수 있는데...
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