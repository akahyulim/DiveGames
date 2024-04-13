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

		// 임시: create constant buffer 
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer,sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	}

	// 스파르탄은 이벤트 컬링 함수에서 구현한다.
	void ViewScreen::Update()
	{
		if (!m_pActiveScene || m_pActiveScene->IsEmpty())
			return;

		for (auto it = m_Renderables.begin(); it != m_Renderables.end(); ++it)
		{
			it->second.clear();
		}

		// 임시
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

		// 반투명 오브젝트는 카메라에서 먼 것 부터 그려야 하므로 정렬이 필요하다고 한다.
		// 스파르탄에서 해당 부분은 아직 찾지 못했다.
	}
	
	void ViewScreen::Render()
	{
		forwardRender();
	}

	void ViewScreen::forwardRender()
	{
		// frustum 갱신
		{
			// 마지막 인자는 카메라에서 볼 수 있는 최대 거리를 뜻하는 듯 하다.
			// 그런데 spartan은 근거리를 설정해놓았다.
			m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());
		}

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
			}
		}

		// bind lights
		// forward light는 light x mesh
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
					pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

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

					// 사실 위의 내용도 여기에서 다 처리할 수 있는데...
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
					pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

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

					// 사실 위의 내용도 여기에서 다 처리할 수 있는데...
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