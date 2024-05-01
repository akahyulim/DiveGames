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


#include "Graphics/Shader.h"
#include "Resource/ResourceManager.h"
#include "Graphics/Texture2D.h"

namespace Dive
{
	ViewScreen::ViewScreen(Camera* pCamera, eRenderPath renderPath)
		: m_pCamera(pCamera)
		, m_RenderPath(renderPath)
	{
		m_pActiveScene = SceneManager::GetActiveScene();

		// 임시: create constant buffer 
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuLightVSBuffer, sizeof(m_cpuLightVSBuffer));

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
		ZeroMemory(&m_cpuLightVSBuffer, sizeof(m_cpuLightVSBuffer));

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

		// update에 어울린다.
		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());

		// constant buffer 처리이지만 이 곳도 나쁘지 않다.
		// 추후 좀 더 생각해보자.
		{
			// frame buffer
			// 매번 갱신할 필요가 없어 위치가 이상해졌지만 bind는 draw에서 한다.
			{
				m_cpuFrameBuffer.view = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
				m_cpuFrameBuffer.projection = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			}

			// camera buffer
			{
				m_cpuCameraBuffer.position = m_pCamera->GetPosition();
				
				DirectX::XMFLOAT4X4 proj;
				DirectX::XMStoreFloat4x4(&proj, m_pCamera->GetProjectionMatrix());
				m_cpuCameraBuffer.perspectiveValue.x = 1.0f / proj._11;
				m_cpuCameraBuffer.perspectiveValue.y = 1.0f / proj._22;
				m_cpuCameraBuffer.perspectiveValue.z = proj._43;
				m_cpuCameraBuffer.perspectiveValue.w = -proj._33;
				
				auto viewInv = DirectX::XMMatrixInverse(nullptr, m_pCamera->GetViewMatrix());
				m_cpuCameraBuffer.viewInverse = DirectX::XMMatrixTranspose(viewInv);

				// 애매하지만 드로우콜마다 한 번만 해도 되는 작업이다.
				auto pCameraBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}
		}
	}
	
	void ViewScreen::Render()
	{
		switch (m_RenderPath)
		{
		case eRenderPath::Forward:
		{
			forwardRender();
			return;
		}
		case eRenderPath::Deferred:
		{
			deferredRender();
			return;
		}
		default:
			return;
		}
	}

	void ViewScreen::forwardRender()
	{
		//passOpaque();
		//passTransparent();

		passShadowGen();
		passOpaqueDraw();
		passTransparentDraw();
	}

	void ViewScreen::deferredRender()
	{
		// set & clear g buffer
		Graphics::SetRenderTargetView(0, Renderer::GetGBufferTexture(eGBuffer::ColorSpecIntensity)->GetRenderTargetView());
		Graphics::SetRenderTargetView(1, Renderer::GetGBufferTexture(eGBuffer::Normal)->GetRenderTargetView());
		Graphics::SetRenderTargetView(2, Renderer::GetGBufferTexture(eGBuffer::SpecPower)->GetRenderTargetView());
		Graphics::SetDepthStencilView(Renderer::GetGBufferTexture(eGBuffer::DepthStencil)->GetDepthStencilView());
		Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);	

		// states
		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
		Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::GBuffer));

		// draw on g buffer
		{
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pOpaque->GetMatrix());

				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				Graphics::SetShader(Renderer::GetShader(eShader::Deferred));

				m_cpuMaterialBuffer.diffuseColor = pMaterial->GetDiffuseColor();
				m_cpuMaterialBuffer.properties = 0;
				m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;
				m_cpuMaterialBuffer.properties |= pMaterial->HasTexture(eTextureUnit::Normal) ? (1U << 1) : 0;

				auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
				pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
				pMaterialBuffer->Bind();

				Graphics::SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
				Graphics::SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));

				pRenderableCom->Draw();
			}

			Graphics::SetRenderTargetView(0, NULL);
			Graphics::SetRenderTargetView(1, NULL);
			Graphics::SetRenderTargetView(2, NULL);
		}

		// draw light
		{
			Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			Graphics::ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());

			Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, Renderer::GetGBufferTexture(eGBuffer::ColorSpecIntensity));
			Graphics::SetTexture(eTextureUnit::GBuffer_Normal, Renderer::GetGBufferTexture(eGBuffer::Normal));
			Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, Renderer::GetGBufferTexture(eGBuffer::SpecPower));
			Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, Renderer::GetGBufferTexture(eGBuffer::DepthStencil));

			// directional lights: 일반적으로 하나만 사용하는 듯 하다.
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightComponent = pLight->GetComponent<Light>();

				if (pLightComponent->GetType() != eLightType::Directional)
					continue;
		
				m_cpuLightBuffer.options = (1U << 0);
				m_cpuLightBuffer.direction = pLight->GetForward();
				m_cpuLightBuffer.color = { pLightComponent->GetColor().x * pLightComponent->GetColor().x
					, pLightComponent->GetColor().y * pLightComponent->GetColor().y
					, pLightComponent->GetColor().z * pLightComponent->GetColor().z };

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				Graphics::SetShader(Renderer::GetShader(eShader::DeferredLights));
				Graphics::SetVertexBuffer(nullptr);
				Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
			Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Additive));

			// point lights
			// additive alpha blending을 사용한다.(이건 directional light가 무조건 존재한다는 가정이다.)
			// rasterizer state는 cull front로 한다.
			// depthstencil state는 greater than equals depth comparision test를 사용한다.
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightComponent = pLight->GetComponent<Light>();

				if (pLightComponent->GetType() != eLightType::Point)
					continue;

				m_cpuLightBuffer.options = (1U << 1);
				m_cpuLightBuffer.position = pLight->GetPosition();
				m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
				m_cpuLightBuffer.color = { pLightComponent->GetColor().x * pLightComponent->GetColor().x
					, pLightComponent->GetColor().y * pLightComponent->GetColor().y
					, pLightComponent->GetColor().z * pLightComponent->GetColor().z };

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				Graphics::SetShader(Renderer::GetShader(eShader::DeferredLights));
				Graphics::SetVertexBuffer(nullptr);
				Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			// spot lights
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightComponent = pLight->GetComponent<Light>();

				if (pLightComponent->GetType() != eLightType::Spot)
					continue;

				m_cpuLightBuffer.options = (1U << 2);
				m_cpuLightBuffer.position = pLight->GetPosition();
				m_cpuLightBuffer.direction = pLight->GetForward();
				m_cpuLightBuffer.rangeRcp = 1.0f / pLightComponent->GetRange();
				m_cpuLightBuffer.outerConeAngle = pLightComponent->GetOuterAngleRadian();
				m_cpuLightBuffer.innerConeAngle = pLightComponent->GetInnerAngleRadian();
				m_cpuLightBuffer.color = { pLightComponent->GetColor().x * pLightComponent->GetColor().x
					, pLightComponent->GetColor().y * pLightComponent->GetColor().y
					, pLightComponent->GetColor().z * pLightComponent->GetColor().z };

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				Graphics::SetShader(Renderer::GetShader(eShader::DeferredLights));
				Graphics::SetVertexBuffer(nullptr);
				Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			Graphics::SetDepthStencilState(nullptr);
			Graphics::SetBlendState(nullptr);

			Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, nullptr);
			Graphics::SetTexture(eTextureUnit::GBuffer_Normal, nullptr);
			Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, nullptr);
			Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, nullptr);
		}
	}

	void ViewScreen::passOpaque()
	{
		// set & clear views
		Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
		Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		// 이건 위치가 좀 애매하다.
		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		// bind lights
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
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pOpaque->GetMatrix());

				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				auto pShader = Renderer::GetShader(eShader::ForwardLight);//pMaterial->GetShader();
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
				pRenderableCom->Draw();
			}
		}
	}
	
	void ViewScreen::passTransparent()
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

			Graphics::SetDepthStencilState(NULL);
			Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Transparent));

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pTransparent->GetMatrix());

				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				auto pShader = Renderer::GetShader(eShader::ForwardLight);//pMaterial->GetShader();
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
				pRenderableCom->Draw();
			}
		}

		Graphics::SetDepthStencilState(NULL);
		Graphics::SetBlendState(NULL);
	}

	void ViewScreen::passShadowGen()
	{
		for (const auto light : m_Renderables[eRenderable::Light])
		{
			auto lightCom = light->GetComponent<Light>();
			if (lightCom->GetType() != eLightType::Spot)
				continue;

			// viewport
			// 텍스쳐마다 뷰포트를 만들어두는 편이 나으려나...?
			RECT rt;
			rt.left = 0;
			rt.right = lightCom->GetShadowMapSize();
			rt.top = 0;
			rt.bottom = lightCom->GetShadowMapSize();
			Graphics::SetViewport(rt);

			// set & clear views
			Graphics::SetRenderTargetView(0, nullptr);
			Graphics::SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
			Graphics::ClearViews(eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

			// set state
			Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::ShadowGen));
			Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));

			// constant buffer를 renderer가 가져야 하는가?
			// 개별 객체에서 관리하면 안되는가?
			m_cpuLightVSBuffer.shadow = DirectX::XMMatrixTranspose(lightCom->GetShaodwMatrix());
			auto pLightVSBuffer = Renderer::GetConstantBuffer(eConstantBuffer::LightVS);
			pLightVSBuffer->Update((void*)&m_cpuLightVSBuffer);
			pLightVSBuffer->Bind();

			// draw opaques''s depth			
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pOpaque->GetMatrix());
				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();

				auto pShader = Renderer::GetShader(eShader::ShadowGen);
				Graphics::SetShader(pShader);

				pRenderableCom->Draw();
			}

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pTransparent->GetMatrix());

				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				auto pShader = Renderer::GetShader(eShader::ForwardLightShadow);
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
				// 적어도 Material 정도는 직접 할 수도 있다.
				pRenderableCom->Draw();
			}
		}

		Graphics::SetRenderTargetView(0, nullptr);
		Graphics::SetDepthStencilView(nullptr);
		Graphics::SetShader(nullptr);
	}

	void ViewScreen::passOpaqueDraw()
	{
		// viewport
		Graphics::SetViewport(m_pCamera->GetViewport());

		// set & clear views
		Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
		Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		for (int i = 0; i != (int)m_Renderables[eRenderable::Light].size(); ++i)
		{
			auto light = m_Renderables[eRenderable::Light][i];
			auto lightCom = light->GetComponent<Light>();

			switch (lightCom->GetType())
			{
			case eLightType::Directional:
				m_cpuLightBuffer.options = (1U << 0);
				m_cpuLightBuffer.direction = light->GetForward();
				break;
			case eLightType::Point:
				m_cpuLightBuffer.options = (1U << 1);
				m_cpuLightBuffer.position = light->GetPosition();
				m_cpuLightBuffer.rangeRcp = 1.0f / lightCom->GetRange();
				break;
			case eLightType::Spot:
				m_cpuLightBuffer.options = (1U << 2);
				m_cpuLightBuffer.position = light->GetPosition();
				m_cpuLightBuffer.direction = light->GetForward();
				m_cpuLightBuffer.rangeRcp = 1.0f / lightCom->GetRange();
				m_cpuLightBuffer.outerConeAngle = lightCom->GetOuterAngleRadian();
				m_cpuLightBuffer.innerConeAngle = lightCom->GetInnerAngleRadian();
				m_cpuLightBuffer.shadow = DirectX::XMMatrixTranspose(lightCom->GetShaodwMatrix());
				break;
			default:
				break;
			}
			m_cpuLightBuffer.color = { lightCom->GetColor().x * lightCom->GetColor().x
			, lightCom->GetColor().y * lightCom->GetColor().y
			, lightCom->GetColor().z * lightCom->GetColor().z };

			auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
			pLightBuffer->Update((void*)&m_cpuLightBuffer);
			pLightBuffer->Bind();

			if (i == 0)
				Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::ForwardLight));
			else if (i == 1)
				Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Additive));
			//Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));

			// draw opaques
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pOpaque->GetMatrix());
				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				auto pShader = Renderer::GetShader(eShader::ForwardLightShadow);
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
				if (lightCom->GetType() == eLightType::Spot)
					Graphics::SetTexture(eTextureUnit::SpotShadowMap, lightCom->GetShadowMap());

				pRenderableCom->Draw();
			}
		}

		Graphics::SetBlendState(nullptr);
		//Graphics::SetRenderTargetView(0, nullptr);
		//Graphics::SetDepthStencilView(nullptr);
		Graphics::SetShader(nullptr);
	}

	// passOpaqueDraw()와 RTV를 공유한다.
	void ViewScreen::passTransparentDraw()
	{
		// viewport
		Graphics::SetViewport(m_pCamera->GetViewport());

		// set & clear views
		//Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		//Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
		//Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
		{
			const auto* light = m_Renderables[eRenderable::Light][i];
			auto* lightCom = light->GetComponent<Light>();

			switch (lightCom->GetType())
			{
			case eLightType::Directional:
				m_cpuLightBuffer.options = (1U << 0);
				m_cpuLightBuffer.direction = light->GetForward();
				break;
			case eLightType::Point:
				m_cpuLightBuffer.options = (1U << 1);
				m_cpuLightBuffer.position = light->GetPosition();
				m_cpuLightBuffer.rangeRcp = 1.0f / lightCom->GetRange();
				break;
			case eLightType::Spot:
				m_cpuLightBuffer.options = (1U << 2);
				m_cpuLightBuffer.position = light->GetPosition();
				m_cpuLightBuffer.direction = light->GetForward();
				m_cpuLightBuffer.rangeRcp = 1.0f / lightCom->GetRange();
				m_cpuLightBuffer.outerConeAngle = lightCom->GetOuterAngleRadian();
				m_cpuLightBuffer.innerConeAngle = lightCom->GetInnerAngleRadian();
				m_cpuLightBuffer.shadow = DirectX::XMMatrixTranspose(lightCom->GetShaodwMatrix());
				break;
			default:
				break;
			}

			m_cpuLightBuffer.color = { lightCom->GetColor().x * lightCom->GetColor().x
				, lightCom->GetColor().y * lightCom->GetColor().y
				, lightCom->GetColor().z * lightCom->GetColor().z };

			auto lightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
			lightBuffer->Update((void*)&m_cpuLightBuffer);
			lightBuffer->Bind();

			Graphics::SetDepthStencilState(NULL);
			Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Transparent));

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(pTransparent->GetMatrix());

				auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
				pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
				pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

				auto pShader = Renderer::GetShader(eShader::ForwardLightShadow);
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
				if (lightCom->GetType() == eLightType::Spot)
					Graphics::SetTexture(eTextureUnit::SpotShadowMap, lightCom->GetShadowMap());

				// 사실 위의 내용도 여기에서 다 처리할 수 있는데...
				pRenderableCom->Draw();
			}
		}

		Graphics::SetBlendState(nullptr);
		Graphics::SetRenderTargetView(0, nullptr);
		Graphics::SetDepthStencilView(nullptr);
		Graphics::SetShader(nullptr);
	}
}