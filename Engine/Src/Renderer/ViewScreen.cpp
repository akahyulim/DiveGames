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

		// ������ ������Ʈ�� ī�޶󿡼� �� �� ���� �׷��� �ϹǷ� ������ �ʿ��ϴٰ� �Ѵ�.
		// ���ĸ�ź���� �ش� �κ��� ���� ã�� ���ߴ�.

		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());
	}
	
	// �ᱹ �� ������ Renderer�� Graphics�� ���� Ȱ���ϱ� ������
	// enum class ��� ������ ��ȿ���̴�.
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

	// pass�� ��ο���(���̴�) ������ ���еȴ�.
	void ViewScreen::forwardRender()
	{
		passLightDepth();
		passOpaqueDraw();
		passTransparentDraw();

		/*
		å������ light, scene, gbuffer ������ ��ü����
		pass�� �޼��带 ������ �̸� ������ path�� �����Ѵ�.
		
		���� passLightDepth�� ����ȭ
		lightManager::PrepareNextShadowLight
		sceneManager::RenderSceneNoShaders

		���� passOpaqueDraw�� ����ȭ
		gBuffer::PreRender
		sceneManager::RenderSceneToGBuffer
		gBuffer::PostRender
		*/
		
		/*
		���ĸ�ź�� ����
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

	// ���ĸ�ź�� opaque�� transparent�� �����ϴ� �� ����.
	// ������ �׸��ڸ��� ���� ���۷� ���
	// ������Ʈ�� ������ ��, ������������ ��ȯ�� �� ���
	void ViewScreen::passLightDepth()
	{
		// ����������Ʈ���� ã�� �� �������� �����Ϸ��� �̰� �ʿ��ϴٰ� �Ѵ�.
		m_pGraphics->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::ShadowGen);	
		
		for (const auto light : m_Renderables[eRenderableType::Light])
		{
			auto pLightCom = light->GetComponent<Light>();
			if (!light->IsActive() || !pLightCom->IsShadowEnabled())
				continue;

			// ����Ÿ��: ��ǻ� spot�� �����ϴ�.
			m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)nullptr, 0);
			m_pGraphics->BindDepthStencilView(pLightCom->GetShadowMap()->GetDepthStencilView());
			m_pGraphics->ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

			switch (pLightCom->GetType())
			{
				// ���� �� ������ ������ �Ͼ�� �ִ�.
			case eLightType::Directional:
			{
				m_pGraphics->SetRasterizerState(eRasterizerStateType::CascadedShadowGen);

				// 3���� viewport
				auto size = pLightCom->GetShadowMapSize();
				D3D11_VIEWPORT viewports[3] = { {0.0f, 0.0f, size, size, 0.0f, 1.0f}, 
					{0.0f, 0.0f, size, size, 0.0f, 1.0f}, {0.0f, 0.0f, size, size, 0.0f, 1.0f} };
				//m_pGraphics->SetViewports(3, viewports);
				//m_pGraphics->GetDeviceContext()->RSSetViewports(3, viewports);
				m_pGraphics->SetViewport(0, 0, size, size);

				// ����Ÿ��: ��ǻ� spot�� �����ϴ�.
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

				// �ø������� �׸��ڰ� �������.
				// ���̵� �ø��� �ؾ� �ϳ�...?
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
		// �ᱹ ViewScreen�� ũ�Ⱑ �Ǿ�� ���� �ʳ�?
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

			// ���⿡���� �� ��° �������� ������ �ȵ� �� ������...
			//auto pLightBuffer = m_pRenderer->GetPSConstantBuffer(ePSConstBufType::Light);
			//pLightBuffer->Update((void*)&pLightCom->GetCBufferPS());
			//m_pGraphics->BindPSCBuffer(pLightBuffer);
			m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

			// �� ��° �������ʹ� ���� ��ο� �� ����� ȥ���Ѵ�.
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
				m_pGraphics->PSBindConstantBuffer(pRenderableCom->GetConstantBufferPS(), 5);	// ���� 1�� �ٲپ�� �Ѵ�.

				// ���� ���÷��� �׷��� �Ű����� ������ �� �����.
				// �������ó�� �ؽ��İ� slot�� ������ ���������.
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
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::GBuffer);		// å�� �� State���� GBuffer���� �����Ѵ�.

		// �̰� ����Ѵ�.
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
			m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);	// ���� �̰� ��������.
			m_pGraphics->SetPixelShader(ePixelShaderType::GBuffer);

			m_pGraphics->SetVertexBuffer(pRenderableCom->GetVertexBuffer());
			m_pGraphics->SetIndexBuffer(pRenderableCom->GetIndexBuffer());

			m_pGraphics->DrawIndexed(pRenderableCom->GetIndexCount());
		}

		// ���⿡�� �ʱ�ȭ�� �ʿ��ϴ�.
		// -> ���� �޼��带 �����ϸ� ���? �ٵ� ClearViews��� �̸��� �򰡸� �� �ִ�.
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 0);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 1);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 2);
	}

	void ViewScreen::passLight()
	{
		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// �б� ����
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

				// ���������� 2�� ������ �����ߴ�.
				// skybox�� �����ִ� �� �ϴ�.
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
				// ���� �� ������ �ű�� ������ �ذ�ƴ�.
				m_pGraphics->SetDepthStencilState(eDepthStencilStateType::NoDepthWriteGreaterStencilMask);// , 2);
				m_pGraphics->SetRasterizerState(eRasterizerStateType::NoDepthClipFront);

				// å���� 1������ ��Ȳ�� 2�� ��︰��.
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

				// BindDomainCBuffer�� �̸� ����?
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
		m_pGraphics->GetDeviceContext()->RSSetState(nullptr);	// ���� ���� �ʿ�

		m_pGraphics->SetVertexShader(eVertexShaderType::Null);
		m_pGraphics->SetHullShader(eHullShaderType::Null);
		m_pGraphics->SetDomainShader(eDomainShaderType::Null);
		m_pGraphics->SetPixelShader(ePixelShaderType::Null);

		// ���� Texture �׽�Ʈ ������ ������ �̸��� �޼��尡 �����Ѵ�.
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::SpotShadowMap);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::CascadeShadowMap);
	}

	// depthStencilState ó���� �̺��� �� �ϴ�.
	// å������ Direcitonal�� Spot & Point�� Depth Func�� �ٸ���.
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
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// �б� ����
		m_pGraphics->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_pGraphics->GetResolutionWidth()), static_cast<float>(m_pGraphics->GetResolutionHeight()));

		// ConstantBuffers
		m_pGraphics->PSBindConstantBuffer(m_pCamera->GetConstantBufferPS(), 0);
		m_pGraphics->PSBindConstantBuffer(pLightCom->GetConstantBufferPS(), 2);

		// gbuffer resources
		// �ƹ������� �̰� ���ε����� �ʴ� �� ����.
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
			m_pGraphics->GetDeviceContext()->RSSetState(nullptr);	// ���� ���� �ʿ�

			m_pGraphics->SetVertexShader(eVertexShaderType::Null);
			m_pGraphics->SetHullShader(eHullShaderType::Null);
			m_pGraphics->SetDomainShader(eDomainShaderType::Null);
			m_pGraphics->SetGeometryShader(eGeometryShaderType::Null);
			m_pGraphics->SetPixelShader(ePixelShaderType::Null);

			// ���� Texture �׽�Ʈ ������ ������ �̸��� �޼��尡 �����Ѵ�.
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::SpotShadowMap);
			m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::CascadeShadowMap);
		}
	}
}