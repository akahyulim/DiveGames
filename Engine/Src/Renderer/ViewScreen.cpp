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
		// ���ĸ�ź�� ��� opaque�� transparent pass������ ������ �и��س���.
		passShadowGen();	// passDepth�� ��︰��. ���ĸ�ź�� �Ű������� ���� opaque�� trnasparent�� �����Ѵ�.
		passOpaqueDraw();
		passTransparentDraw();

		// urho�� ��� path�� ����ȭ �Ǿ� �ִ�.
		// commnad type�̶�� ū pass �ȿ� ���� pass Ÿ���� �ְ� ���� ������ �Ű������� �޴´�.
		// ���� �������� forward�� ���
		// clear, scenepass(base), forwardlights(light), scenepass(postopaque), scenepass(refract), scenepass(alpha), scenepass(postalpha)�� �����ȴ�.
		// �̰͵��� �Լ�ȭ�ϰ� Path�� ������� �����Ͽ� ȣ���ϵ��� ����.

		// Clear: color, rendertargets(dsv ����)�� �ް� clear
		// ScenePass
		// ForwardLights: rende shadow maps + opaque objects' additive lgihting�̶���.
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

			// directional lights: �Ϲ������� �ϳ��� ����ϴ� �� �ϴ�.
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
			// additive alpha blending�� ����Ѵ�.(�̰� directional light�� ������ �����Ѵٴ� �����̴�.)
			// rasterizer state�� cull front�� �Ѵ�.
			// depthstencil state�� greater than equals depth comparision test�� ����Ѵ�.
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

			// �ϴ� �ȱ׸��ٴ� ���� �׸��ڸ��� ������� �ʴ´ٴ� �ǹ̴�.
			// ������ �ʱ�ȭ�� ������� �Ѵ�. 
			// �׷��� �ʴٸ� �׸��ڸ��� �ѱ�� �κп��� �ٽ� ��, ���� �Ǵ��ؾ� �Ѵ�.
			// => ���ҽ� ���������� �װ� �� ��������...
			if (lightCom->GetLightShadows() == eLightShadows::None)
			{
				Graphics::SetRenderTargetView(0, nullptr);
				Graphics::SetDepthStencilView(lightCom->GetShadowMap()->GetDepthStencilView());
				Graphics::ClearViews(eClearFlags::Depth, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
				continue;
			}

			// viewport
			// �ؽ��ĸ��� ����Ʈ�� �����δ� ���� ��������...?
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
				// shadow���� ���θ� �߰��� �� �ִ�.
				// ��, �׸��ڸ� ����� ���� ���� ���̶�� �ǳ� �� �� �ְ� �ϸ� �ȴ�.
				// => ã�ƺ��� ����Ƽ���� ����� ��, �޴� �� �� ���� ���еǾ� �ִ�.

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

	// passOpaqueDraw()�� RTV�� �����Ѵ�.
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

				// ��� ���� ���뵵 ���⿡�� �� ó���� �� �ִµ�...
				pRenderableCom->Draw();
			}
		}

		Graphics::SetBlendState(nullptr);
		Graphics::SetRenderTargetView(0, nullptr);
		Graphics::SetDepthStencilView(nullptr);
		Graphics::SetShader(nullptr);
	}
}