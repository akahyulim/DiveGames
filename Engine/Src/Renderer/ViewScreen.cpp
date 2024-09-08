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
	ViewScreen::ViewScreen(Camera* pCamera, eRenderPathType renderPath)
		: m_pGraphics(Graphics::GetInstance())
		, m_pRenderer(Renderer::GetInstance())
		, m_pCamera(pCamera)
		, m_RenderPath(renderPath)
	{
		DV_ENGINE_ASSERT(m_pGraphics->IsInitialized());
		DV_ENGINE_ASSERT(m_pRenderer->IsInitialized());

		m_pActiveScene = GetSceneManager()->GetActiveScene();

		m_GBuffer.Initialize(m_pGraphics->GetResolutionWidth(), m_pGraphics->GetResolutionHeight());
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
					m_Renderables[pMaterial->IsOpaque() ? eRenderableType::Opaque : eRenderableType::Transparent].emplace_back(pGameObject);
				}
			}

			//if (auto pLightCom = pGameObject->GetComponent<Light>())
			{
			//	m_Renderables[eRenderableType::Light].emplace_back(pGameObject);
			}

			if (auto pLightCom = pGameObject->GetComponent<Light>())
			{
				m_Renderables[eRenderableType::Light].emplace_back(pGameObject);
			}
		}

		// 반투명 오브젝트는 카메라에서 먼 것 부터 그려야 하므로 정렬이 필요하다고 한다.
		// 스파르탄에서 해당 부분은 아직 찾지 못했다.

		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());
	}
	
	// 결국 이 곳에서 Renderer와 Graphics를 전부 활용하기 때문에
	// enum class 사용 과정이 비효율이다.
	void ViewScreen::Render()
	{
		switch (m_RenderPath)
		{
		case eRenderPathType::Forward:
		{
			forwardRender();
			return;
		}
		case eRenderPathType::Deferred:
		{
			deferredRender();
			return;
		}
		default:
			return;
		}
	}

	// pass는 드로우콜(셰이더) 단위로 구분된다.
	void ViewScreen::forwardRender()
	{
		passLightDepth();
		passOpaqueDraw();
		passTransparentDraw();

		/*
		책에서는 light, scene, gbuffer 각자의 객체에서
		pass별 메서드를 가지며 이를 조합해 path를 구성한다.
		
		현재 passLightDepth를 세분화
		lightManager::PrepareNextShadowLight
		sceneManager::RenderSceneNoShaders

		현재 passOpaqueDraw를 세분화
		gBuffer::PreRender
		sceneManager::RenderSceneToGBuffer
		gBuffer::PostRender
		*/
		
		/*
		스파르탄의 구성
		Pass_Visibility(cmd_list);
        Pass_Depth_Prepass(cmd_list, false);
        Pass_GBuffer(cmd_list);
        Pass_Ssgi(cmd_list);
        Pass_Ssr(cmd_list, rt_render);
        Pass_Sss(cmd_list);
        Pass_Light(cmd_list);                        // compute diffuse and specular buffers
        Pass_Light_Composition(cmd_list, rt_render); // compose diffuse, specular, ssgi, volumetric etc.
        Pass_Light_ImageBased(cmd_list, rt_render);  // apply IBL and SSR
		
		Pass_Depth_Prepass(cmd_list, do_transparent_pass);
        Pass_GBuffer(cmd_list, do_transparent_pass);
        Pass_Ssr(cmd_list, rt_render, do_transparent_pass);
        Pass_Light(cmd_list, do_transparent_pass);
        Pass_Light_Composition(cmd_list, rt_render, do_transparent_pass);
        Pass_Light_ImageBased(cmd_list, rt_render, do_transparent_pass);
		*/
	}

	// 스파르탄은 opaque와 transparent를 구분하는 것 같다.
	// 광원의 그림자맵을 깊이 버퍼로 사용
	// 오브젝트를 광원의 뷰, 프로젝션으로 변환한 후 기록
	void ViewScreen::passLightDepth()
	{
		// 참고프로젝트에선 찾을 수 없었지만 코파일럿은 이게 필요하다고 한다.
		m_pGraphics->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::ShadowGen);	
		
		for (const auto light : m_Renderables[eRenderableType::Light])
		{
			auto pLightCom = light->GetComponent<Light>();
			if (!light->IsActive() || !pLightCom->IsShadowEnabled())
				continue;

			// 렌더타겟: 사실상 spot과 동일하다.
			m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)nullptr, 0);
			m_pGraphics->BindDepthStencilView(pLightCom->GetShadowMap()->GetDepthStencilView());
			m_pGraphics->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

			switch (pLightCom->GetType())
			{
				// 역시 이 곳에서 문제가 일어나고 있다.
			case eLightType::Directional:
			{
				m_pGraphics->SetRasterizerState(eRasterizerStateType::CascadedShadowGen);

				// 3개의 viewport
				auto size = pLightCom->GetShadowMapSize();
				D3D11_VIEWPORT viewports[3] = { {0.0f, 0.0f, size, size, 0.0f, 1.0f}, 
					{0.0f, 0.0f, size, size, 0.0f, 1.0f}, {0.0f, 0.0f, size, size, 0.0f, 1.0f} };
				//m_pGraphics->SetViewports(3, viewports);
				//m_pGraphics->GetDeviceContext()->RSSetViewports(3, viewports);
				m_pGraphics->SetViewport(0, 0, size, size);

				// 렌더타겟: 사실상 spot과 동일하다.
				//m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)nullptr, 0);
				//m_pGraphics->BindDepthStencilView(pLightCom->GetShadowMap()->GetDepthStencilView());
				//m_pGraphics->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				
				m_pGraphics->GSBindConstantBuffer(pLightCom->GetConstantBufferGS(), 2);

				m_pGraphics->SetVertexShader(eVertexShaderType::DirLightDepth);
				m_pGraphics->SetHullShader(eHullShaderType::Null);
				m_pGraphics->SetDomainShader(eDomainShaderType::Null);
				m_pGraphics->SetGeometryShader(eGeometryShaderType::CascadeShadowMaps);
				m_pGraphics->SetPixelShader(ePixelShaderType::Null);
				break;
			}
			case eLightType::Spot:
			{
				m_pGraphics->SetRasterizerState(eRasterizerStateType::Count);

				// set & clear views
				//m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)nullptr, 0);
				//m_pGraphics->BindDepthStencilView(pLightCom->GetShadowMap()->GetDepthStencilView());
				//m_pGraphics->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

				// viewport
				m_pGraphics->SetViewport(0, 0, pLightCom->GetShadowMapSize(), pLightCom->GetShadowMapSize());

				m_pGraphics->VSBindConstantBuffer(pLightCom->GetConstantBufferVS(), 2);

				m_pGraphics->SetVertexShader(eVertexShaderType::LightDepth);
				m_pGraphics->SetHullShader(eHullShaderType::Null);
				m_pGraphics->SetDomainShader(eDomainShaderType::Null);
				m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);
				m_pGraphics->SetPixelShader(ePixelShaderType::Null);
				break;
			}
			default:
				continue;
			}

			// draw opaque's depth			
			for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				// 컬링때문에 그림자가 사라진다.
				// 깊이도 컬링을 해야 하나...?
				//if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
				//	continue;

				m_pGraphics->VSBindConstantBuffer(pRenderableCom->GetConstantBufferVS(), 1);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());
				
				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}

			/*
			// draw transparent's depth
			for (const auto* pTransparent : m_Renderables[eRenderableType::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_pGraphics->VSBindConstantBuffer(pRenderableCom->GetConstantBufferVS(), 1);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}
			*/
		}
	}

	void ViewScreen::passOpaqueDraw()
	{
		// 결국 ViewScreen의 크기가 되어야 하지 않나?
		m_pGraphics->SetViewport(0, 0, (float)m_pGraphics->GetResolutionWidth(), (float)m_pGraphics->GetResolutionHeight());

		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_pGraphics->GetDefaultDepthStencilView());
		m_pGraphics->ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		m_pGraphics->SetRasterizerState(eRasterizerStateType::FillSolid_CullBack);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::DepthEqual);

		m_pGraphics->SetVertexShader(eVertexShaderType::ForwardLight);
		m_pGraphics->SetPixelShader(ePixelShaderType::ForwardLight);

		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Linear), 0);
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);

		for (int i = 0; i != (int)m_Renderables[eRenderableType::Light].size(); ++i)
		{
			auto light = m_Renderables[eRenderableType::Light][i];
			auto pLightCom = light->GetComponent<Light>();

			// 여기에서도 두 번째 광원부터 갱신이 안될 것 같은데...
			//auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			//pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			//m_pGraphics->BindPSCBuffer(pLightBuffer);
			m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

			// 두 번째 광원부터는 이전 드로우 콜 결과를 혼합한다.
			if(i == 1)
				m_pGraphics->SetBlendState(eBlendStateType::Additive);
			
			// draw opaques
			for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_pGraphics->VSBindConstantBuffer(pRenderableCom->GetConstantBufferVS(), 1);
				m_pGraphics->PSBindConstantBuffer(pRenderableCom->GetConstantBufferPS(), 5);	// 추후 1로 바꾸어야 한다.

				// 위의 샘플러도 그렇고 매개변수 구성이 좀 별루다.
				// 상수버퍼처럼 텍스쳐가 slot을 가지면 깔끔해진다.
				m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Diffuse), eTextureUnitType::Diffuse);
				m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Normal), eTextureUnitType::Normal);
				if (pLightCom->GetType() == eLightType::Spot)
					m_pGraphics->BindPSResource(pLightCom->GetShadowMap(), eTextureUnitType::SpotShadowMap);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}
		}

		m_pGraphics->SetBlendState(eBlendStateType::Null);
	}

	void ViewScreen::passTransparentDraw()
	{
		m_pGraphics->SetVertexShader(eVertexShaderType::ForwardLight);
		m_pGraphics->SetPixelShader(ePixelShaderType::ForwardLight);

		m_pGraphics->SetRasterizerState(eRasterizerStateType::FillSolid_CullNone);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::Null);
		m_pGraphics->SetBlendState(eBlendStateType::Transparent);

		for (int i = 0; i < (int)m_Renderables[eRenderableType::Light].size(); i++)
		{
			const auto* light = m_Renderables[eRenderableType::Light][i];
			auto* pLightCom = light->GetComponent<Light>();

			//auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			//pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			//m_pGraphics->BindPSCBuffer(pLightBuffer);
			m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderableType::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				m_pGraphics->VSBindConstantBuffer(pRenderableCom->GetConstantBufferVS(), 1);
				m_pGraphics->PSBindConstantBuffer(pRenderableCom->GetConstantBufferPS(), 5);

				m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Diffuse), eTextureUnitType::Diffuse);
				m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Normal), eTextureUnitType::Normal);
				if (pLightCom->GetType() == eLightType::Spot)
					m_pGraphics->BindPSResource(pLightCom->GetShadowMap(), eTextureUnitType::SpotShadowMap);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}
		}
	}

	void ViewScreen::passGBuffer()
	{
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_GBuffer.GetWidth()), static_cast<float>(m_GBuffer.GetHeight()));

		// set & clear g buffer
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetDiffuseTex()->GetRenderTargetView(), 0);
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetNormalTex()->GetRenderTargetView(), 1);
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetSpecularTex()->GetRenderTargetView(), 2);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilView());
		m_pGraphics->ClearViews(eClearFlags::Color | eClearFlags::Depth | eClearFlags::Stencil, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// states
		m_pGraphics->SetRasterizerState(eRasterizerStateType::FillSolid_CullBack);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::GBuffer);		// 책은 이 State까지 GBuffer에서 관리한다.

		// 이걸 사용한다.
		m_pGraphics->VSBindConstantBuffer(m_pCamera->GetConstantBufferVS(), 0);

		// draw on g buffer
		for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
		{
			auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
			const auto* pMaterial = pRenderableCom->GetMaterial();
			const auto& boundingBox = pRenderableCom->GetBoundingBox();

			if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
				continue;

			m_pGraphics->VSBindConstantBuffer(pRenderableCom->GetConstantBufferVS(), 1);
			m_pGraphics->PSBindConstantBuffer(pRenderableCom->GetConstantBufferPS(), 5);

			m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Diffuse), eTextureUnitType::Diffuse);
			m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Normal), eTextureUnitType::Normal);

			m_pGraphics->SetVertexShader(eVertexShaderType::GBuffer);
			m_pGraphics->SetHullShader(eHullShaderType::Null);
			m_pGraphics->SetDomainShader(eDomainShaderType::Null);
			m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);	// 역시 이게 문제였다.
			m_pGraphics->SetPixelShader(ePixelShaderType::GBuffer);

			m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
			m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

			m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
		}

		// 여기에선 초기화가 필요하다.
		// -> 차라리 메서드를 제공하면 어떨까? 근데 ClearViews라는 이름은 헷가릴 수 있다.
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 0);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 1);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 2);
	}

	void ViewScreen::passLight()
	{
		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// 읽기 전용
		m_pGraphics->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_pGraphics->GetResolutionWidth()), static_cast<float>(m_pGraphics->GetResolutionHeight()));

		// ConstantBuffers
		m_pGraphics->PSBindConstantBuffer(m_pCamera->GetConstantBufferPS(), 0);

		m_pGraphics->BindPSResource(m_GBuffer.GetDiffuseTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource(m_GBuffer.GetNormalTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource(m_GBuffer.GetSpecularTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource(m_GBuffer.GetDepthTex()->GetDepthShaderResourceView(), eTextureUnitType::GBuffer_DepthStencil);

		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Linear), 0);
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);
		
		for (int i = 0; i < static_cast<int>(m_Renderables[eRenderableType::Light].size()); i++)
		{
			const auto* pLight = m_Renderables[eRenderableType::Light][i];
			auto* pLightCom = pLight->GetComponent<Light>();

			if (!pLight->IsActive())
				continue;

			m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

			if(i >= 1)
			{
				m_pGraphics->SetBlendState(eBlendStateType::Additive);
			}

			switch (pLightCom->GetType())
			{
			case eLightType::Directional:
			{
				//m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);

				// 예제에서는 2를 변수로 관리했다.
				// skybox와 관련있는 듯 하다.
				m_pGraphics->SetDepthStencilState(eDepthStencilStateType::NoDepthWriteLessStencilMask);// , 2);
				
				m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

				if (pLightCom->IsShadowEnabled())
					m_pGraphics->BindPSResource(pLightCom->GetShadowMap()->GetDepthShaderResourceView(), eTextureUnitType::CascadeShadowMap);
				
				m_pGraphics->SetVertexShader(eVertexShaderType::DeferredDirLight);
				m_pGraphics->SetHullShader(eHullShaderType::Null);
				m_pGraphics->SetDomainShader(eDomainShaderType::Null);
				m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);
				m_pGraphics->SetPixelShader(ePixelShaderType::DeferredDirLight);

				m_pGraphics->SetVertexBuffer(nullptr);
				m_pGraphics->Draw(4, 0);
				break;
			}
			
			case eLightType::Point:
			{
				// 역시 이 곳으로 옮기니 문제가 해결됐다.
				m_pGraphics->SetDepthStencilState(eDepthStencilStateType::NoDepthWriteGreaterStencilMask);// , 2);
				m_pGraphics->SetRasterizerState(eRasterizerStateType::NoDepthClipFront);

				// 책에는 1이지만 정황상 2가 어울린다.
				m_pGraphics->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

				m_pGraphics->SetVertexShader(eVertexShaderType::DeferredPointLight);
				m_pGraphics->SetHullShader(eHullShaderType::DeferredPointLight);
				m_pGraphics->SetDomainShader(eDomainShaderType::DeferredPointLight);
				m_pGraphics->SetPixelShader(ePixelShaderType::DeferredPointLight);

				m_pGraphics->DSBindConstantBuffer(m_pCamera->GetConstantBufferDS(), 0);
				m_pGraphics->DSBindConstantBuffer(pLightCom->GetConstantBufferDS(), 1);

				m_pGraphics->SetVertexBuffer(nullptr);
				m_pGraphics->Draw(2, 0);
				break;
			}
			
			case eLightType::Spot:
			{
				//m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);

				m_pGraphics->SetDepthStencilState(eDepthStencilStateType::NoDepthWriteGreaterStencilMask);// , 2);
				m_pGraphics->SetRasterizerState(eRasterizerStateType::NoDepthClipFront);

				if(pLightCom->IsShadowEnabled())
					m_pGraphics->BindPSResource(pLightCom->GetShadowMap()->GetDepthShaderResourceView(), eTextureUnitType::SpotShadowMap);

				m_pGraphics->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

				m_pGraphics->SetVertexShader(eVertexShaderType::DeferredSpotLight);
				m_pGraphics->SetHullShader(eHullShaderType::DeferredSpotLight);
				m_pGraphics->SetDomainShader(eDomainShaderType::DeferredSpotLight);
				m_pGraphics->SetPixelShader(ePixelShaderType::DeferredSpotLight);

				// BindDomainCBuffer로 이름 변경?
				m_pGraphics->DSBindConstantBuffer(m_pCamera->GetConstantBufferDS(), 0);
				m_pGraphics->DSBindConstantBuffer(pLightCom->GetConstantBufferDS(), 1);

				m_pGraphics->SetVertexBuffer(nullptr);
				m_pGraphics->Draw(1, 0);
				break;
			}

			default:
				break;
			}
		}
	
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::Null);
		m_pGraphics->SetBlendState(eBlendStateType::Null);
		m_pGraphics->GetDeviceContext()->RSSetState(nullptr);	// 추후 수정 필요

		m_pGraphics->SetVertexShader(eVertexShaderType::Null);
		m_pGraphics->SetHullShader(eHullShaderType::Null);
		m_pGraphics->SetDomainShader(eDomainShaderType::Null);
		m_pGraphics->SetPixelShader(ePixelShaderType::Null);

		// 현재 Texture 테스트 때문에 동일한 이름의 메서드가 존재한다.
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::SpotShadowMap);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::CascadeShadowMap);
	}

	// depthStencilState 처리가 미비한 듯 하다.
	// 책에서는 Direcitonal과 Spot & Point의 Depth Func가 다르다.
	void ViewScreen::deferredRender()
	{
		passLightDepth();
		passGBuffer();
		passLight();
		//passDirLight();
	}

	void ViewScreen::passDirLight()
	{
		const auto* pLight = m_Renderables[eRenderableType::Light][0];
		auto* pLightCom = pLight->GetComponent<Light>();

		if (!pLight->IsActive() || pLightCom->GetType() != eLightType::Directional)
			return;

		// rendertargets
		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// 읽기 전용
		m_pGraphics->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_pGraphics->GetResolutionWidth()), static_cast<float>(m_pGraphics->GetResolutionHeight()));

		// ConstantBuffers
		m_pGraphics->PSBindConstantBuffer(m_pCamera->GetConstantBufferPS(), 0);
		m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

		// gbuffer resources
		// 아무리봐도 이게 바인딩되지 않는 것 같다.
		m_pGraphics->BindPSResource(m_GBuffer.GetDepthTex()->GetDepthShaderResourceView(), eTextureUnitType::GBuffer_DepthStencil);
		m_pGraphics->BindPSResource(m_GBuffer.GetDiffuseTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource(m_GBuffer.GetNormalTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource(m_GBuffer.GetSpecularTex()->GetColorShaderResourceView(), eTextureUnitType::GBuffer_Specular);

		// samplers
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Linear), 0);
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);

		// states	
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::NoDepthWriteLessStencilMask);
		//m_pGraphics->SetBlendState(eBlendStateType::Null);
		//m_pGraphics->GetDeviceContext()->RSSetState(nullptr);

		m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		if (pLightCom->IsShadowEnabled())
			m_pGraphics->BindPSResource(pLightCom->GetShadowMap()->GetDepthShaderResourceView(), eTextureUnitType::CascadeShadowMap);

		// shaders
		m_pGraphics->SetVertexShader(eVertexShaderType::DeferredDirLight);
		m_pGraphics->SetHullShader(eHullShaderType::Null);
		m_pGraphics->SetDomainShader(eDomainShaderType::Null);
		m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);
		m_pGraphics->SetPixelShader(ePixelShaderType::DeferredDirLight);

		m_pGraphics->SetVertexBuffer(nullptr);
		m_pGraphics->Draw(4, 0);
			
		{
			m_pGraphics->SetDepthStencilState(eDepthStencilStateType::Null);
			m_pGraphics->SetBlendState(eBlendStateType::Null);
			m_pGraphics->GetDeviceContext()->RSSetState(nullptr);	// 추후 수정 필요

			m_pGraphics->SetVertexShader(eVertexShaderType::Null);
			m_pGraphics->SetHullShader(eHullShaderType::Null);
			m_pGraphics->SetDomainShader(eDomainShaderType::Null);
			m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);
			m_pGraphics->SetPixelShader(ePixelShaderType::Null);

			// 현재 Texture 테스트 때문에 동일한 이름의 메서드가 존재한다.
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::SpotShadowMap);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::CascadeShadowMap);
		}
	}
}