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

		// ������ ������Ʈ�� ī�޶󿡼� �� �� ���� �׷��� �ϹǷ� ������ �ʿ��ϴٰ� �Ѵ�.
		// ���ĸ�ź���� �ش� �κ��� ���� ã�� ���ߴ�.

		// update�� ��︰��.
		m_Frustum.Construct(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m_pCamera->GetFarClipPlane());

		// constant buffer ó�������� �� ���� ������ �ʴ�.
		// ���� �� �� �����غ���.
		// => ��� �� ������ �����ϸ� �ȵȴ�.
		// constant buffer�� ���̴��� �ͼӵ� �����⿡ ���������� ���ε� �Ǿ�� �Ѵ�.
		{
			// ���� ���
			// frame buffer
			// �Ź� ������ �ʿ䰡 ���� ��ġ�� �̻��������� bind�� draw���� �Ѵ�.
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
		passDepth();
		passOpaqueDraw();
		passTransparentDraw();

		/*
		å������ light, scene, gbuffer ������ ��ü����
		pass�� �޼��带 ������ �̸� ������ path�� �����Ѵ�.
		
		���� passDepth�� ����ȭ
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

				// �ø������� �׸��ڰ� �������.
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
		// �ᱹ ViewScreen�� ũ�Ⱑ �Ǿ�� ���� �ʳ�?
		m_pGraphics->SetViewport(0, 0, (float)m_pGraphics->GetResolutionWidth(), (float)m_pGraphics->GetResolutionHeight());

		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_pGraphics->GetDefaultDepthStencilView());
		m_pGraphics->ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

		m_pGraphics->SetRasterizerState(eRasterizerStateType::FillSolid_CullBack);
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::DepthEqual);

		m_pGraphics->SetVertexShader(eVertexShaderType::ForwardLight);
		m_pGraphics->SetPixelShader(ePixelShaderType::ForwardLight);

		// 1. ���÷��� �� �پ��ؾ� �Ѵ�. ����Ƽ�� ���� ��� point, linear, trilinear �� ���� ���͸� ��尡 �����Ѵ�.
		// 2. srv�� ���� ����ϴ� ���÷��� �޶�� �Ѵ�.(�� ���� ����Ƽ�� �����̴�)
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

			// �ε����� �ƴ϶� Ȱ��ȭ�� ������ ������ �ľ��ؾ� �Ѵ�.
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
		m_pGraphics->SetDepthStencilState(eDepthStencilStateType::GBuffer);		// å�� �� State���� GBuffer���� �����Ѵ�.

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

		// ���⿡�� �ʱ�ȭ�� �ʿ��ϴ�.
		// -> ���� �޼��带 �����ϸ� ���? �ٵ� ClearViews��� �̸��� �򰡸� �� �ִ�.
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 0);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 1);
		m_pGraphics->BindRenderTargetView((ID3D11RenderTargetView*)NULL, 2);
	}

	void ViewScreen::passLight()
	{
		m_pGraphics->BindRenderTargetView(m_pCamera->GetRenderTargetView(), 0);
		m_pGraphics->BindDepthStencilView(m_GBuffer.GetDepthTex()->GetDepthStencilViewReadOnly());	// ���� Ŭ��� �ʿ��� �� �ϴ�.
		m_pGraphics->ClearViews(eClearFlags::Color, m_pCamera->GetBackgroundColor());
		m_pGraphics->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_pGraphics->SetViewport(0, 0, static_cast<float>(m_pGraphics->GetResolutionWidth()), static_cast<float>(m_pGraphics->GetResolutionHeight()));

		// å������ ����Ÿ�� ���ε� �� 
		// GBuffer::PrepareForUnpack()�̶�� �Ͽ� 
		// ī�޶��� �������� ���, �� ������� ��� ���۷� �����Ѵ�.
		// �ϴ� ���� ���� �����ߴ�.
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
		// å������ �ϴ� Directional Light������ BlendState�� ������� �ʴ´�.
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
		// additive alpha blending�� ����Ѵ�.(�̰� directional light�� ������ �����Ѵٴ� �����̴�.)
		// rasterizer state�� cull front�� �Ѵ�.
		// depthstencil state�� greater than equals depth comparision test�� ����Ѵ�.
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

		// ���� Texture �׽�Ʈ ������ ������ �̸��� �޼��尡 �����Ѵ�.
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Diffuse);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_Specular);
		m_pGraphics->BindPSResource((Texture*)nullptr, eTextureUnitType::GBuffer_DepthStencil);
	}

	// depthStencilState ó���� �̺��� �� �ϴ�.
	// å������ Direcitonal�� Spot & Point�� Depth Func�� �ٸ���.
	void ViewScreen::deferredRender()
	{
		passGBuffer();
		passLight();
	}
}