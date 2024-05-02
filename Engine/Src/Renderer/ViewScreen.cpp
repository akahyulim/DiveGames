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
	}

	// 스파르탄은 이벤트 컬링 함수에서 구현한다.
	void ViewScreen::Update()
	{
		if (!m_pActiveScene || m_pActiveScene->IsEmpty())
			return;

		// 새롭게 추가
		// 하나의 Scene에 View가 여러개라면 문제가 된다.
		m_pActiveScene->Update();

		for (auto it = m_Renderables.begin(); it != m_Renderables.end(); ++it)
		{
			it->second.clear();
		}

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
				auto pCameraBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Camera);
				pCameraBuffer->Update((void*)&m_pCamera->GetCBufferVS());
				pCameraBuffer->Bind();
			}

			// camera buffer
			{
				auto pCameraBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Camera);
				pCameraBuffer->Update((void*)&m_pCamera->GetCBufferPS());
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
		// 스파르탄의 경우 opaque와 transparent pass묶음을 완전히 분리해놨다.
		passShadowGen();	// passDepth가 어울린다. 스파르탄은 매개변수를 통해 opaque와 trnasparent를 구분한다.
		passOpaqueDraw();
		passTransparentDraw();

		// urho의 경우 path가 파일화 되어 있다.
		// commnad type이라는 큰 pass 안에 세부 pass 타입이 있고 이후 설정을 매개변수로 받는다.
		// 가장 기초적인 forward의 경우
		// clear, scenepass(base), forwardlights(light), scenepass(postopaque), scenepass(refract), scenepass(alpha), scenepass(postalpha)로 구성된다.
		// 이것들을 함수화하고 Path에 순서대로 구성하여 호출하도록 하자.

		// Clear: color, rendertargets(dsv 포함)를 받고 clear
		// ScenePass
		// ForwardLights: rende shadow maps + opaque objects' additive lgihting이란다.
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

				auto pModelBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				Graphics::SetShader(Renderer::GetShader(eShader::Deferred));

				auto pMaterialBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Material);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
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
				auto* pLightCom = pLight->GetComponent<Light>();

				if (pLightCom->GetType() != eLightType::Directional)
					continue;
	
				auto pLightBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
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
				auto* pLightCom = pLight->GetComponent<Light>();

				if (pLightCom->GetType() != eLightType::Point)
					continue;

				auto pLightBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
				pLightBuffer->Bind();

				Graphics::SetShader(Renderer::GetShader(eShader::DeferredLights));
				Graphics::SetVertexBuffer(nullptr);
				Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			// spot lights
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightCom = pLight->GetComponent<Light>();

				if (pLightCom->GetType() != eLightType::Spot)
					continue;

				auto pLightBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
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

	void ViewScreen::passShadowGen()
	{
		// set state
		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::ShadowGen));
		Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));

		auto pShader = Renderer::GetShader(eShader::ShadowGen);
		Graphics::SetShader(pShader);

		for (const auto light : m_Renderables[eRenderable::Light])
		{
			auto lightCom = light->GetComponent<Light>();
			if (lightCom->GetType() != eLightType::Spot)
				continue;

			// 일단 안그린다는 것은 그림자맵을 사용하지 않는다는 의미다.
			// 하지만 초기화는 시켜줘야 한다. 
			// 그렇지 않다면 그림자맵을 넘기는 부분에서 다시 유, 무를 판단해야 한다.
			// => 리소스 차원에서는 그게 더 나을수도...
			if (lightCom->GetLightShadows() == eLightShadows::None)
			{
				Graphics::SetRenderTargetView(0, nullptr);
				Graphics::SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
				Graphics::ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				continue;
			}

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
			Graphics::ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
		
			auto pLightVSBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Light);
			pLightVSBuffer->Update((void*)&lightCom->GetCBufferVS());
			pLightVSBuffer->Bind();
			
			// draw opaque's depth			
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				// shadow적용 여부를 추가할 수 있다.
				// 즉, 그림자를 만들고 싶지 않은 모델이라면 건너 띌 수 있게 하면 된다.
				// => 찾아보니 유니티에는 만드는 것, 받는 것 두 개가 구분되어 있다.

				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				pRenderableCom->Draw();
			}

			// draw transparent's depth
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

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

		auto pShader = Renderer::GetShader(eShader::ForwardLightShadow);
		Graphics::SetShader(pShader);

		for (int i = 0; i != (int)m_Renderables[eRenderable::Light].size(); ++i)
		{
			auto light = m_Renderables[eRenderable::Light][i];
			auto lightCom = light->GetComponent<Light>();
			
			auto pLightBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Light);
			pLightBuffer->Update((void*)&lightCom->GetCBufferPS());
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
			
				auto pModelBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				auto pMaterialBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Material);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
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

		auto pShader = Renderer::GetShader(eShader::ForwardLightShadow);
		Graphics::SetShader(pShader);

		Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
		{
			const auto* light = m_Renderables[eRenderable::Light][i];
			auto* lightCom = light->GetComponent<Light>();
	
			auto lightBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Light);
			lightBuffer->Update((void*)&lightCom->GetCBufferPS());
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

				auto pModelBuffer = Renderer::GetVSConstantBuffer(eVSConstantBuffers::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();
				
				auto pMaterialBuffer = Renderer::GetPSConstantBuffer(ePSConstantBuffers::Material);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
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