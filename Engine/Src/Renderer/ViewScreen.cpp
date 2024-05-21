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
#include "Graphics/Pipeline.h"

namespace Dive
{
	ViewScreen::ViewScreen(Camera* pCamera, eRenderPath renderPath)
		: m_pCamera(pCamera)
		, m_RenderPath(renderPath)
	{
		m_pActiveScene = GetSceneManager()->GetActiveScene();
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
		// => 사실 이 곳에서 구현하면 안된다.
		// constant buffer는 셰이더에 귀속된 정보기에 렌더링에서 바인딩 되어야 한다.
		{
			// frame buffer
			// 매번 갱신할 필요가 없어 위치가 이상해졌지만 bind는 draw에서 한다.
			{
				auto pCameraBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Camera);
				pCameraBuffer->Update((void*)&m_pCamera->GetCBufferVS());
				pCameraBuffer->Bind();
			}

			// camera buffer
			{
				auto pCameraBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Camera);
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
		passDepthNew();
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

	// pass마다 Pipeline을 구성하고 drawcall 후 초기화하는 것이 디폴트인듯 하다.
	// 하지만 pass로는구분되어 있지만 pipeline의 일부 요소는 공유해야 하는 경우가 존재한다.
	void ViewScreen::passDepth()
	{
		// set state
		GetGraphics()->SetRasterizerState(GetRenderer()->GetRasterizerState(eRasterizerState::ShadowGen));
		GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::DepthReadWrite));

		auto pVertexShader = GetRenderer()->GetShader(eRendererShader::Depth_VS);
		pVertexShader->Bind();

		for (const auto light : m_Renderables[eRenderable::Light])
		{
			auto lightCom = light->GetComponent<Light>();
			if (lightCom->GetType() != eLightType::Spot)
				continue;

			// 일단 안그린다는 것은 그림자맵을 사용하지 않는다는 의미다.
			// 하지만 초기화는 시켜줘야 한다. 
			// 그렇지 않다면 그림자맵을 넘기는 부분에서 다시 유, 무를 판단해야 한다.
			// => 리소스 차원에서는 그게 더 나을수도...
			if (!lightCom->IsShadowEnabled())
			{
				GetGraphics()->SetRenderTargetView(0, nullptr);
				GetGraphics()->SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
				GetGraphics()->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				continue;
			}

			// viewport
			// 텍스쳐마다 뷰포트를 만들어두는 편이 나으려나...?
			RECT rt;
			rt.left = 0;
			rt.right = static_cast<LONG>(lightCom->GetShadowMapSize());
			rt.top = 0;
			rt.bottom = static_cast<LONG>(lightCom->GetShadowMapSize());
			GetGraphics()->SetViewport(rt);

			// set & clear views
			GetGraphics()->SetRenderTargetView(0, nullptr);
			GetGraphics()->SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
			GetGraphics()->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

			auto pLightVSBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Light);
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

				// 컬링때문에 그림자가 사라진다.
				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
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

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				pRenderableCom->Draw();
			}
		}

		GetGraphics()->SetShader(eShaderType::Vertex, nullptr);
		GetGraphics()->SetRenderTargetView(0, nullptr);
		GetGraphics()->SetDepthStencilView(nullptr);
	}

	void ViewScreen::passDepthNew()
	{
		auto pRenderer = GetRenderer();

		Pipeline pipeline(GetGraphics()->GetDeviceContext());
		pipeline.BindRasterizerState(pRenderer->GetRasterizerState(eRasterizerState::ShadowGen));
		pipeline.BindDepthStencilState(pRenderer->GetDepthStencilState(eDepthStencilState::DepthReadWrite));
		pipeline.BindVertexShader(pRenderer->GetShader(eRendererShader::Depth_VS)->GetVertexShader());
		pipeline.BindInputLayout(pRenderer->GetShader(eRendererShader::Depth_VS)->GetInputLayout());
		pipeline.BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		{
			auto pCameraBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Camera);
			pCameraBuffer->Update((void*)&m_pCamera->GetCBufferVS());
			//pCameraBuffer->Bind();
			pipeline.BindVertexShaderCBuffer(pCameraBuffer->GetBufferNew(), pCameraBuffer->GetSlot());
		}

		// camera buffer
		{
			auto pCameraBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Camera);
			pCameraBuffer->Update((void*)&m_pCamera->GetCBufferPS());
			//pCameraBuffer->Bind();
			pipeline.BindPixelShaderCBuffer(pCameraBuffer->GetBufferNew(), pCameraBuffer->GetSlot());
		}

		for (const auto light : m_Renderables[eRenderable::Light])
		{
			auto lightCom = light->GetComponent<Light>();
			if (lightCom->GetType() != eLightType::Spot)
				continue;

			// set & clear views
			//GetGraphics()->SetRenderTargetView(0, nullptr);
			//GetGraphics()->SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
			//GetGraphics()->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
			pipeline.BindRenderTargetView(nullptr, 0);
			pipeline.BindDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
			pipeline.ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

			// 일단 안그린다는 것은 그림자맵을 사용하지 않는다는 의미다.
			// 하지만 초기화는 시켜줘야 한다. 
			// 그렇지 않다면 그림자맵을 넘기는 부분에서 다시 유, 무를 판단해야 한다.
			// => 리소스 차원에서는 그게 더 나을수도...
			if (!lightCom->IsShadowEnabled())
			{
				//GetGraphics()->SetRenderTargetView(0, nullptr);
				//GetGraphics()->SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
				//GetGraphics()->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				continue;
			}

			// viewport
			// 텍스쳐마다 뷰포트를 만들어두는 편이 나으려나...?
			RECT rt;
			rt.left = 0;
			rt.right = static_cast<LONG>(lightCom->GetShadowMapSize());
			rt.top = 0;
			rt.bottom = static_cast<LONG>(lightCom->GetShadowMapSize());
			//GetGraphics()->SetViewport(rt);
			pipeline.BindViewport(0, 0, lightCom->GetShadowMapSize(), lightCom->GetShadowMapSize(), 0.0f, 1.0f);

			

			auto pLightVSBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Light);
			pLightVSBuffer->Update((void*)&lightCom->GetCBufferVS());
			//pLightVSBuffer->Bind();
			pipeline.BindVertexShaderCBuffer(pLightVSBuffer->GetBufferNew(), pLightVSBuffer->GetSlot());

			// draw opaque's depth			
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				// shadow적용 여부를 추가할 수 있다.
				// 즉, 그림자를 만들고 싶지 않은 모델이라면 건너 띌 수 있게 하면 된다.
				// => 찾아보니 유니티에는 만드는 것, 받는 것 두 개가 구분되어 있다.

				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				// 컬링때문에 그림자가 사라진다.
				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				//pModelBuffer->Bind();
				pipeline.BindVertexShaderCBuffer(pModelBuffer->GetBufferNew(), pModelBuffer->GetSlot());

				//pipeline.SetPipeline();

				//pRenderableCom->Draw();
				//Graphics::GetInstance()->SetVertexBuffer(GetVertexBuffer());
				//Graphics::GetInstance()->SetIndexBuffer(GetIndexBuffer());
				//Graphics::GetInstance()->DrawIndexed(topology, m_IndexCount, m_IndexOffset);

				pipeline.BindVertexBuffer(pRenderableCom->GetVertexBuffer());
				pipeline.BindIndexBuffer(pRenderableCom->GetIndexBuffer());
				
				pipeline.DrawIndexed(pRenderableCom->GetIndexCount());
			}

			/*
			// draw transparent's depth
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				pRenderableCom->Draw();
			}
			*/
		}

		//pipeline.Clear();

	}

	void ViewScreen::passOpaqueDraw()
	{
		// viewport
		GetGraphics()->SetViewport(m_pCamera->GetViewport());

		// set & clear views
		GetGraphics()->SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		GetGraphics()->SetDepthStencilView(GetGraphics()->GetDefaultDepthStencilView());
		GetGraphics()->ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		GetGraphics()->SetRasterizerState(GetRenderer()->GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		auto pVertexShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_VS);
		pVertexShader->Bind();
		auto pPixelShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_PS);
		pPixelShader->Bind();

		for (int i = 0; i != (int)m_Renderables[eRenderable::Light].size(); ++i)
		{
			auto light = m_Renderables[eRenderable::Light][i];
			auto lightCom = light->GetComponent<Light>();

			auto pLightBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Light);
			pLightBuffer->Update((void*)&lightCom->GetCBufferPS());
			pLightBuffer->Bind();

			if (i == 0)
				GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::ForwardLight));
			else if (i == 1)
				GetGraphics()->SetBlendState(GetRenderer()->GetBlendState(eBlendState::Additive));
			//GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::DepthReadWrite));

			// draw opaques
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				auto pMaterialBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Model);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
				pMaterialBuffer->Bind();

				GetGraphics()->SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
				GetGraphics()->SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));
				if (lightCom->GetType() == eLightType::Spot)
					GetGraphics()->SetTexture(eTextureUnit::SpotShadowMap, lightCom->GetShadowMap());

				pRenderableCom->Draw();
			}
		}

		GetGraphics()->SetBlendState(nullptr);
		//GetGraphics()->SetRenderTargetView(0, nullptr);
		//GetGraphics()->SetDepthStencilView(nullptr);
	}

	// passOpaqueDraw()와 RTV를 공유한다.
	void ViewScreen::passTransparentDraw()
	{
		// viewport
		GetGraphics()->SetViewport(m_pCamera->GetViewport());

		// set & clear views
		//GetGraphics()->SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		//GetGraphics()->SetDepthStencilView(GetGraphics()->GetDefaultDepthStencilView());
		//GetGraphics()->ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		auto pVertexShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_VS);
		pVertexShader->Bind();
		auto pPixelShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_PS);
		pPixelShader->Bind();

		GetGraphics()->SetRasterizerState(GetRenderer()->GetRasterizerState(eRasterizerState::FillSolid_CullBack));

		for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
		{
			const auto* light = m_Renderables[eRenderable::Light][i];
			auto* lightCom = light->GetComponent<Light>();

			auto lightBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Light);
			lightBuffer->Update((void*)&lightCom->GetCBufferPS());
			lightBuffer->Bind();

			GetGraphics()->SetDepthStencilState(NULL);
			GetGraphics()->SetBlendState(GetRenderer()->GetBlendState(eBlendState::Transparent));

			// draw transparents
			for (const auto* pTransparent : m_Renderables[eRenderable::Transparent])
			{
				auto* pRenderableCom = pTransparent->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				auto pMaterialBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Model);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
				pMaterialBuffer->Bind();

				GetGraphics()->SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
				GetGraphics()->SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));
				if (lightCom->GetType() == eLightType::Spot)
					GetGraphics()->SetTexture(eTextureUnit::SpotShadowMap, lightCom->GetShadowMap());

				// 사실 위의 내용도 여기에서 다 처리할 수 있는데...
				pRenderableCom->Draw();
			}
		}

		GetGraphics()->SetBlendState(nullptr);
		GetGraphics()->SetRenderTargetView(0, nullptr);
		GetGraphics()->SetDepthStencilView(nullptr);
	}

	void ViewScreen::deferredRender()
	{
		// set & clear g buffer
		GetGraphics()->SetRenderTargetView(0, GetRenderer()->GetGBufferTexture(eGBuffer::ColorSpecIntensity)->GetRenderTargetView());
		GetGraphics()->SetRenderTargetView(1, GetRenderer()->GetGBufferTexture(eGBuffer::Normal)->GetRenderTargetView());
		GetGraphics()->SetRenderTargetView(2, GetRenderer()->GetGBufferTexture(eGBuffer::SpecPower)->GetRenderTargetView());
		GetGraphics()->SetDepthStencilView(GetRenderer()->GetGBufferTexture(eGBuffer::DepthStencil)->GetDepthStencilView());
		GetGraphics()->ClearViews(eClearFlags::Color | eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);	

		// states
		GetGraphics()->SetRasterizerState(GetRenderer()->GetRasterizerState(eRasterizerState::FillSolid_CullBack));
		GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::GBuffer));

		// draw on g buffer
		{
			for (const auto* pOpaque : m_Renderables[eRenderable::Opaque])
			{
				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				if (!m_Frustum.IsVisible(boundingBox.GetCenter(), boundingBox.GetExtent()))
					continue;

				auto pModelBuffer = GetRenderer()->GetVSConstantBuffer(eVSConstBuf::Model);
				pModelBuffer->Update((void*)&pRenderableCom->GetCBufferVS());
				pModelBuffer->Bind();

				auto pVertexShader = GetRenderer()->GetShader(eRendererShader::GBuffer_VS);
				pVertexShader->Bind();
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::GBuffer_PS);
				pPixelShader->Bind();

				auto pMaterialBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Model);
				pMaterialBuffer->Update((void*)&pRenderableCom->GetCBufferPS());
				pMaterialBuffer->Bind();

				GetGraphics()->SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));
				GetGraphics()->SetTexture(eTextureUnit::Normal, pMaterial->GetTexture(eTextureUnit::Normal));

				pRenderableCom->Draw();
			}

			GetGraphics()->SetRenderTargetView(0, NULL);
			GetGraphics()->SetRenderTargetView(1, NULL);
			GetGraphics()->SetRenderTargetView(2, NULL);
		}

		// draw light
		{
			GetGraphics()->SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			GetGraphics()->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());

			GetGraphics()->SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, GetRenderer()->GetGBufferTexture(eGBuffer::ColorSpecIntensity));
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_Normal, GetRenderer()->GetGBufferTexture(eGBuffer::Normal));
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_SpecPower, GetRenderer()->GetGBufferTexture(eGBuffer::SpecPower));
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_DepthStencil, GetRenderer()->GetGBufferTexture(eGBuffer::DepthStencil));

			// directional lights: 일반적으로 하나만 사용하는 듯 하다.
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightCom = pLight->GetComponent<Light>();

				if (pLightCom->GetType() != eLightType::Directional)
					continue;
	
				auto pLightBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
				pLightBuffer->Bind();

				auto pVertexShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_VS);
				pVertexShader->Bind();
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				pPixelShader->Bind();

				GetGraphics()->SetVertexBuffer(nullptr);
				GetGraphics()->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::ForwardLight));
			GetGraphics()->SetBlendState(GetRenderer()->GetBlendState(eBlendState::Additive));

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

				auto pLightBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
				pLightBuffer->Bind();

				auto pVertexShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_VS);
				pVertexShader->Bind();
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				pPixelShader->Bind();

				GetGraphics()->SetVertexBuffer(nullptr);
				GetGraphics()->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			// spot lights
			for (int i = 0; i < (int)m_Renderables[eRenderable::Light].size(); i++)
			{
				const auto* pLight = m_Renderables[eRenderable::Light][i];
				auto* pLightCom = pLight->GetComponent<Light>();

				if (pLightCom->GetType() != eLightType::Spot)
					continue;

				auto pLightBuffer = GetRenderer()->GetPSConstantBuffer(ePSConstBuf::Light);
				pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
				pLightBuffer->Bind();

				auto pVertexShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_VS);
				pVertexShader->Bind();
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				pPixelShader->Bind();

				GetGraphics()->SetVertexBuffer(nullptr);
				GetGraphics()->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			GetGraphics()->SetDepthStencilState(nullptr);
			GetGraphics()->SetBlendState(nullptr);

			GetGraphics()->SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, nullptr);
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_Normal, nullptr);
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_SpecPower, nullptr);
			GetGraphics()->SetTexture(eTextureUnit::GBuffer_DepthStencil, nullptr);
		}
	}
}