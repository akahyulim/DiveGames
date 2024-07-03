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

					pMaterial->IsOpaque() ? m_OpaqueModels.emplace_back(pGameObject) : m_TransparentModels.emplace_back(pGameObject);
				}
			}

			if (auto pLightCom = pGameObject->GetComponent<Light>())
			{
				m_Renderables[eRenderableType::Light].emplace_back(pGameObject);

				switch (pLightCom->GetType())
				{
				case eLightType::Directional:
					m_pDirectionalLight = pGameObject;
					break;

				case eLightType::Point:
					m_PointLights.emplace_back(pGameObject);
					break;

				case eLightType::Spot:
					m_SpotLights.emplace_back(pGameObject);
					break;

				default:
					break;
				}
			}
		}

		// 반투명 오브젝트는 카메라에서 먼 것 부터 그려야 하므로 정렬이 필요하다고 한다.
		// 스파르탄에서 해당 부분은 아직 찾지 못했다.

		// update에 어울린다.
		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());

		// constant buffer 처리이지만 이 곳도 나쁘지 않다.
		// 추후 좀 더 생각해보자.
		// => 사실 이 곳에서 구현하면 안된다.
		// constant buffer는 셰이더에 귀속된 정보기에 렌더링에서 바인딩 되어야 한다.
		{
			// 제거 대상
			// frame buffer
			// 매번 갱신할 필요가 없어 위치가 이상해졌지만 bind는 draw에서 한다.
			{
			//	auto pCameraBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Camera);
			//	pCameraBuffer->Update((void*)&m_pCamera->GetCBufferVS());
			//	pCameraBuffer->Bind();
			}

			// camera buffer
			{
			//	auto pCameraBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Camera);
			//	pCameraBuffer->Update((void*)&m_pCamera->GetCBufferPS());
			//	pCameraBuffer->Bind();
			}
		}
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
		passDepth();
		passOpaqueDraw();
		passTransparentDraw();

		/*
		책에서는 light, scene, gbuffer 각자의 객체에서
		pass별 메서드를 가지며 이를 조합해 path를 구성한다.
		
		현재 passDepth를 세분화
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

	void ViewScreen::passDepth()
	{
		m_pGraphics->SetRasterizerState(eRasterizerStateType::ShadowGen);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::DepthLess);
		m_pGraphics->SetVertexShader(eVertexShaderType::LightDepth);
		m_pGraphics->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto pVSCBufferCamera = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Camera);
		pVSCBufferCamera->Update((void*)&m_pCamera->GetCBufferVS());
		m_pGraphics->BindVSCBuffer(pVSCBufferCamera);

		auto pPSCBufferCamera = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Camera);
		pPSCBufferCamera->Update((void*)&m_pCamera->GetCBufferPS());
		m_pGraphics->BindPSCBuffer(pPSCBufferCamera); 

		for (const auto light : m_Renderables[eRenderableType::Light])
		{
			auto pLightCom = light->GetComponent<Light>();
			if (pLightCom->GetType() != eLightType::Spot)
				continue;

			// set & clear views
			m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)nullptr, 0);
			m_pGraphics->BindDepthStencilView(pLightCom->GetShadowMap()->GetDepthStencilView());
			m_pGraphics->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

			if (!pLightCom->IsShadowEnabled())
				continue;

			// viewport
			m_pGraphics->SetViewport(0, 0, pLightCom->GetShadowMapSize(), pLightCom->GetShadowMapSize());

			auto pLightVSBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Light);
			pLightVSBuffer->Update((void*)&pLightCom->GetCBufferVS());
			m_pGraphics->BindVSCBuffer(pLightVSBuffer);

			// draw opaque's depth			
			for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				// 컬링때문에 그림자가 사라진다.
				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				m_pGraphics->BindVSCBuffer(pModelBuffer);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());
				
				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}

			// draw transparent's depth
			for (const auto* pTransparent : m_Renderables[eRenderableType::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				m_pGraphics->BindVSCBuffer(pModelBuffer);

				m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
				m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

				m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
			}
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

		// 1. 샘플러는 더 다양해야 한다. 유니티를 예로 들면 point, linear, trilinear 세 가지 필터링 모드가 존재한다.
		// 2. srv에 따라 사용하는 샘플러가 달라야 한다.(이 역시 유니티의 구성이다)
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Linear), 0);
		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Pcf), 1);

		for (int i = 0; i != (int)m_Renderables[eRenderableType::Light].size(); ++i)
		{
			auto light = m_Renderables[eRenderableType::Light][i];
			auto pLightCom = light->GetComponent<Light>();

			if (!pLightCom->IsEnabled())
				continue;

			auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pLightBuffer);

			// 인덱스가 아니라 활성화된 광원의 개수를 파악해야 한다.
			if (i == 0)
				m_pGraphics->SetBlendState(eBlendStateType::Null);
			else if (i == 1)
				m_pGraphics->SetBlendState(eBlendStateType::Additive);
			
			// draw opaques
			for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				m_pGraphics->BindVSCBuffer(pModelBuffer);

				auto pMaterialBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Model);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
				m_pGraphics->BindPSCBuffer(pMaterialBuffer);

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

			if (!pLightCom->IsEnabled())
				continue;

			auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pLightBuffer);

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderableType::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				m_pGraphics->BindVSCBuffer(pModelBuffer);

				auto pMaterialBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Model);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
				m_pGraphics->BindPSCBuffer(pMaterialBuffer);

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
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetDiffuseTex(), 0);
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetNormalTex(), 1);
		m_pGraphics->BindRenderTargetView(m_GBuffer.GetSpecularTex(), 2);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex());
		m_pGraphics->ClearViews(eClearFlags::Color | eClearFlags::Depth | eClearFlags::Stencil, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// states
		m_pGraphics->SetRasterizerState(eRasterizerStateType::FillSolid_CullBack);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::GBuffer);		// 책은 이 State까지 GBuffer에서 관리한다.

		// draw on g buffer
		for (const auto* pOpaque : m_Renderables[eRenderableType::Opaque])
		{
			auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
			const auto* pMaterial = pRenderableCom->GetMaterial();
			const auto& boundingBox = pRenderableCom->GetBoundingBox();

			if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
				continue;

			auto pModelBuffer = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Model);
			pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
			pModelBuffer->Bind();

			m_pGraphics->SetVertexShader(eVertexShaderType::GBuffer);
			m_pGraphics->SetPixelShader(ePixelShaderType::GBuffer);

			auto pMaterialBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Model);
			pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pMaterialBuffer);

			m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Diffuse), eTextureUnitType::Diffuse);
			m_pGraphics->BindPSResource(pMaterial->GetTexture(eTextureUnitType::Normal), eTextureUnitType::Normal);

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
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// 따로 클리어가 필요한 듯 하다.
		m_pGraphics->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());
		m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_pGraphics->GetResolutionWidth()), static_cast<float>(m_pGraphics->GetResolutionHeight()));

		// 책에서는 렌더타겟 바인드 후 
		// GBuffer::PrepareForUnpack()이라고 하여 
		// 카메라의 프로젝션 행렬, 뷰 역행렬을 상수 버퍼로 전달한다.
		// 일단 나는 직접 전달했다.
		auto pVSCBufferCamera = m_pRenderer->GetVSConstantBuffer(eVSConstBufType::Camera);
		pVSCBufferCamera->Update((void*)&m_pCamera->GetCBufferVS());
		m_pGraphics->BindVSCBuffer(pVSCBufferCamera);

		auto pPSCBufferCamera = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Camera);
		pPSCBufferCamera->Update((void*)&m_pCamera->GetCBufferPS());
		m_pGraphics->BindPSCBuffer(pPSCBufferCamera);

		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::Light);
	
		m_pGraphics->BindPSResource(m_GBuffer.GetDiffuseTex(), eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource(m_GBuffer.GetNormalTex(), eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource(m_GBuffer.GetSpecularTex(), eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource(m_GBuffer.GetDepthTex(), eTextureUnitType::GBuffer_DepthStencil);

		m_pGraphics->BindPSSampler(m_pRenderer->GetSampler(eSamplerType::Linear), 0);
		// 책에서는 일단 Directional Light에서는 BlendState를 사용하지 않는다.
		//m_pGraphics->SetBlendState(eBlendStateType::Additive);

		if(m_pDirectionalLight)
		{
			auto* pLightCom = m_pDirectionalLight->GetComponent<Light>();
			auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pLightBuffer);

			m_pGraphics->SetVertexShader(eVertexShaderType::DeferredLight);
			m_pGraphics->SetPixelShader(ePixelShaderType::DeferredLight);

			m_pGraphics->SetVertexBuffer(nullptr);
			m_pGraphics->Draw(4, 0);
		}

		//m_pGraphics->SetDepthStencilState(eDepthStencilStateType::DepthEqual);
		//m_pGraphics->SetBlendState(eBlendStateType::Null);
		
		/*
		// point lights
		// additive alpha blending을 사용한다.(이건 directional light가 무조건 존재한다는 가정이다.)
		// rasterizer state는 cull front로 한다.
		// depthstencil state는 greater than equals depth comparision test를 사용한다.
		for (int i = 0; i < (int)m_Renderables[eRenderableType::Light].size(); i++)
		{
			const auto* pLight = m_Renderables[eRenderableType::Light][i];
			auto* pLightCom = pLight->GetComponent<Light>();

			if (pLightCom->GetType() != eLightType::Point)
				continue;

			auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pLightBuffer);

			m_pGraphics->SetVertexShader(eVertexShaderType::DeferredLight);
			m_pGraphics->SetPixelShader(ePixelShaderType::DeferredLight);

			m_pGraphics->SetVertexBuffer(nullptr);
			m_pGraphics->Draw(4, 0);
		}

		// spot lights
		for (int i = 0; i < (int)m_Renderables[eRenderableType::Light].size(); i++)
		{
			const auto* pLight = m_Renderables[eRenderableType::Light][i];
			auto* pLightCom = pLight->GetComponent<Light>();

			if (pLightCom->GetType() != eLightType::Spot)
				continue;

			auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			m_pGraphics->BindPSCBuffer(pLightBuffer);

			m_pGraphics->SetVertexShader(eVertexShaderType::DeferredLight);
			m_pGraphics->SetPixelShader(ePixelShaderType::DeferredLight);

			m_pGraphics->SetVertexBuffer(nullptr);
			m_pGraphics->Draw(4, 0);
		}
		*/
		//m_pGraphics->SetDepthStencilState(nullptr);
		m_pGraphics->SetBlendState(eBlendStateType::Null);

		// 현재 Texture 테스트 때문에 동일한 이름의 메서드가 존재한다.
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
	}

	// depthStencilState 처리가 미비한 듯 하다.
	// 책에서는 Direcitonal과 Spot & Point의 Depth Func가 다르다.
	void ViewScreen::deferredRender()
	{
		passGBuffer();
		passLight();
	}
}