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
		m_pActiveScene = GetSceneManager()->GetActiveScene();
	}

	// ���ĸ�ź�� �̺�Ʈ �ø� �Լ����� �����Ѵ�.
	void ViewScreen::Update()
	{
		if (!m_pActiveScene || m_pActiveScene->IsEmpty())
			return;

		// ���Ӱ� �߰�
		// �ϳ��� Scene�� View�� ��������� ������ �ȴ�.
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

		// ������ ������Ʈ�� ī�޶󿡼� �� �� ���� �׷��� �ϹǷ� ������ �ʿ��ϴٰ� �Ѵ�.
		// ���ĸ�ź���� �ش� �κ��� ���� ã�� ���ߴ�.

		// update�� ��︰��.
		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());

		// constant buffer ó�������� �� ���� ������ �ʴ�.
		// ���� �� �� �����غ���.
		{
			// frame buffer
			// �Ź� ������ �ʿ䰡 ���� ��ġ�� �̻��������� bind�� draw���� �Ѵ�.
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
		passDepth();
		passOpaqueDraw();
		passTransparentDraw();

		/*
		���ĸ�ź�� ������ ������ �� ���� �� ����.
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
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::GBuffer_PS);
				GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
				GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

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

			// directional lights: �Ϲ������� �ϳ��� ����ϴ� �� �ϴ�.
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
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
				GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

				GetGraphics()->SetVertexBuffer(nullptr);
				GetGraphics()->Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);
			}

			GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::ForwardLight));
			GetGraphics()->SetBlendState(GetRenderer()->GetBlendState(eBlendState::Additive));

			// point lights
			// additive alpha blending�� ����Ѵ�.(�̰� directional light�� ������ �����Ѵٴ� �����̴�.)
			// rasterizer state�� cull front�� �Ѵ�.
			// depthstencil state�� greater than equals depth comparision test�� ����Ѵ�.
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
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
				GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

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
				auto pPixelShader = GetRenderer()->GetShader(eRendererShader::DeferredLight_PS);
				GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
				GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

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

	// pass���� Pipeline�� �����ϰ� drawcall �� �ʱ�ȭ�ϴ� ���� ����Ʈ�ε� �ϴ�.
	// ������ pass�δ±��еǾ� ������ pipeline�� �Ϻ� ��Ҵ� �����ؾ� �ϴ� ��찡 �����Ѵ�.
	void ViewScreen::passDepth()
	{
		// set state
		GetGraphics()->SetRasterizerState(GetRenderer()->GetRasterizerState(eRasterizerState::ShadowGen));
		GetGraphics()->SetDepthStencilState(GetRenderer()->GetDepthStencilState(eDepthStencilState::DepthReadWrite));

		auto pVertexShader = GetRenderer()->GetShader(eRendererShader::Depth_VS);
		GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);

		for (const auto light : m_Renderables[eRenderable::Light])
		{
			auto lightCom = light->GetComponent<Light>();
			if (lightCom->GetType() != eLightType::Spot)
				continue;

			// �ϴ� �ȱ׸��ٴ� ���� �׸��ڸ��� ������� �ʴ´ٴ� �ǹ̴�.
			// ������ �ʱ�ȭ�� ������� �Ѵ�. 
			// �׷��� �ʴٸ� �׸��ڸ��� �ѱ�� �κп��� �ٽ� ��, ���� �Ǵ��ؾ� �Ѵ�.
			// => ���ҽ� ���������� �װ� �� ��������...
			if(!lightCom->IsShadowEnabled())
			{
				GetGraphics()->SetRenderTargetView(0, nullptr);
				GetGraphics()->SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
				GetGraphics()->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				continue;
			}

			// viewport
			// �ؽ��ĸ��� ����Ʈ�� �����δ� ���� ��������...?
			RECT rt;
			rt.left = 0;
			rt.right = lightCom->GetShadowMapSize();
			rt.top = 0;
			rt.bottom = lightCom->GetShadowMapSize();
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
				// shadow���� ���θ� �߰��� �� �ִ�.
				// ��, �׸��ڸ� ����� ���� ���� ���̶�� �ǳ� �� �� �ְ� �ϸ� �ȴ�.
				// => ã�ƺ��� ����Ƽ���� ����� ��, �޴� �� �� ���� ���еǾ� �ִ�.

				auto* pRenderableCom = pOpaque->GetComponent<Renderable>();
				const auto* pMaterial = pRenderableCom->GetMaterial();
				const auto& boundingBox = pRenderableCom->GetBoundingBox();

				// �ø������� �׸��ڰ� �������.
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

		GetGraphics()->SetRenderTargetView(0, nullptr);
		GetGraphics()->SetDepthStencilView(nullptr);
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
		auto pPixelShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_PS);
		GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
		GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

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

	// passOpaqueDraw()�� RTV�� �����Ѵ�.
	void ViewScreen::passTransparentDraw()
	{
		// viewport
		GetGraphics()->SetViewport(m_pCamera->GetViewport());

		// set & clear views
		//GetGraphics()->SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
		//GetGraphics()->SetDepthStencilView(GetGraphics()->GetDefaultDepthStencilView());
		//GetGraphics()->ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		auto pVertexShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_VS);
		auto pPixelShader = GetRenderer()->GetShader(eRendererShader::ForwardLight_PS);
		GetGraphics()->SetShader(eShaderType::Vertex, pVertexShader);
		GetGraphics()->SetShader(eShaderType::Pixel, pPixelShader);

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

				// ��� ���� ���뵵 ���⿡�� �� ó���� �� �ִµ�...
				pRenderableCom->Draw();
			}
		}

		GetGraphics()->SetBlendState(nullptr);
		GetGraphics()->SetRenderTargetView(0, nullptr);
		GetGraphics()->SetDepthStencilView(nullptr);
	}
}