#include "divepch.h"
#include "Layer.h"
#include "Renderer.h"
#include "Skydome.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ConstantBuffer.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

// ����Ű�� ����
// Layer�� ������ ��ü�� �����ϸ� �ʱ�ȭ�Լ��� �ټ��� �ݹ��Լ��� �����ȴ�.
// �׸��� ��ü Layer���� �ش� �Լ����� �����Ͽ� ����Ѵ�.
// ��ü Layer�� �⺻������ Scene�� Rendere�� ������
// �ݹ� �Լ��� �ϵ��� �̿��� ó���� �����Ѵ�.
// ���⿡�� Renderer�� ���� Graphics + Renderer�� ����� ������ Ŭ������
// ���ε��Լ��� ���� ���ҽ� ����(��� ����)���� �����Ѵ�.
namespace Dive
{
    Layer::Layer()
    {
    }

    Layer::~Layer()
    {
        // �ܼ��ϰ� ���� �� ���̾�� �� �ϳ��� ���� �����ϴ� ���� �´�.
        // ������ ������ ���� �ٸ� ���̾�� �׸� �� �� �ִ�.
    }
    
    void Layer::Update()
    {
		m_pCamera = nullptr;
        m_Renderables.clear();

        auto allGameObjects = Scene::GetAllGameObjects();
        for (auto* pGameObject : allGameObjects)
        {
			// ī�޶� ������ Ȥ�� �ٸ� �Լ��� ���� ���� ���� ���� �ִ�.
            if (Camera* pCamera = pGameObject->GetComponent<Camera>())
            {
				// ���� ī�޶� �������� �� ������ ���̾�� �ϳ��� �����ϴ� �� ���� �ʳ�?
                m_Renderables[eRendererGameObject::Camera].emplace_back(pGameObject);
                m_pCamera = pCamera;
            }

            if (Light* pLight = pGameObject->GetComponent<Light>())
            {
                m_Renderables[eRendererGameObject::Light].emplace_back(pGameObject);
            }

            if (MeshRenderer* pMeshRenderer = pGameObject->GetComponent<MeshRenderer>())
            {
                m_Renderables[eRendererGameObject::Geometry].emplace_back(pGameObject);
            }

            if (SkinnedMeshRenderer* pSkinnedMeshRenderer = pGameObject->GetComponent<SkinnedMeshRenderer>())
            {
                m_Renderables[eRendererGameObject::SkinnedGeometry].emplace_back(pGameObject);
            }
        }
    }

    void Layer::Render()
    {
        deferredRendering();  
    }

	// ���� Graphics�� ���ε� �Լ��� ���� Context�� �޾ƿ� ���ε� �ϴ� �Լ��� �����ִ�.
	// Graphics�� �ʿ��Ѱ���� ������ ���. 
	// sparky�� ��� ������ bind, draw �Լ����� dxcontext�κ��� devicecontext�� ������ ���� ȣ���Ѵ�.
	// �׷��� ������ SetRenderTarget()�� ���� ã�� ���ߴٴ°Ŵ�... => �̰� ������Ʈ�� �ܼ��Զ����� �� �ϴ�.
    void Layer::deferredRendering()
    {
		// cmdClear
		{
			ID3D11RenderTargetView* pRenderTextures[] = {
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_Color_SpecIntensity)->GetRenderTargetView(),
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_Normal)->GetRenderTargetView(),
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_SpecPower)->GetRenderTargetView()
			};
			Graphics::SetRenderTargetViews(0, 3, pRenderTextures);
			Graphics::SetDepthStencilView(Renderer::GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)->GetDepthStencilView());
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil, m_pCamera->GetBackgroundColor(), 1.0, 0);
		}

		// cmdScenePass - Deferred
		{
			ID3D11RenderTargetView* pRenderTextures[] = {
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_Color_SpecIntensity)->GetRenderTargetView(),
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_Normal)->GetRenderTargetView(),
				Renderer::GetRenderTarget(eRenderTarget::GBuffer_SpecPower)->GetRenderTargetView()
			};
			Graphics::SetRenderTargetViews(0, 3, pRenderTextures);
			Graphics::SetDepthStencilView(Renderer::GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)->GetDepthStencilView());

			const D3D11_VIEWPORT viewport = m_pCamera->GetViewport();
			Dive::Graphics::GetDeviceContext()->RSSetViewports(1, &viewport);

			auto pMappedData = static_cast<CameraVertexShaderBuffer*>(Renderer::GetCameraVertexShaderBuffer()->Map());
			pMappedData->cameraMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetWorldMatrix());
			pMappedData->viewMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
			pMappedData->projMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			Renderer::GetCameraVertexShaderBuffer()->Unmap();
			Dive::Graphics::SetConstantBuffer(0, Dive::eShaderType::VertexShader, Renderer::GetCameraVertexShaderBuffer());

			if (m_pCamera->GetSkydome())
				m_pCamera->GetSkydome()->Render();

			Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::GBuffer), 1);
			// �⺻������ �ù��� ������ plane�̳� triangleó�� �ϳ��� ������ ������� �޽ô� �� �ָ��ϴ�.
			Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

			// MeshRenderer �׸���
			for (auto* pMeshRenderer : m_Renderables[eRendererGameObject::Geometry])
			{
				pMeshRenderer->GetComponent<MeshRenderer>()->Draw();
			}
			for (auto* pSkinnedMeshRenderer : m_Renderables[eRendererGameObject::SkinnedGeometry])
			{
				pSkinnedMeshRenderer->GetComponent<SkinnedMeshRenderer>()->Draw();
			}

			ID3D11RenderTargetView* pRenderTargetViews[] = { nullptr, nullptr, nullptr };
			Graphics::SetRenderTargetViews(0, 3, pRenderTargetViews);
			Graphics::SetDepthStencilView(Renderer::GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)->GetDepthStencilViewReadOnly());
		}
		
		// cmdLightVolumes - DeferredLight
		{
			Graphics::SetRenderTargetView(0, Renderer::GetRenderTarget(eRenderTarget::FrameRender)->GetRenderTargetView());
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());	// �̰� �� ����?
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

			// camera - ��Ÿ ����(���� ����)
			//DirectX::XMFLOAT3 pos;
			auto pBuffer = Renderer::GetCameraPixelShaderBuffer();
			auto pMappedData = static_cast<CameraPixelShaderBuffer*>(pBuffer->Map());
			//DirectX::XMStoreFloat3(&pos, m_pCamera->GetGameObject()->GetTransform()->GetPosition());
			pMappedData->cameraPos = m_pCamera->GetTransform()->GetPosition();//pos;
			DirectX::XMFLOAT4X4 proj;
			DirectX::XMStoreFloat4x4(&proj, m_pCamera->GetProjectionMatrix());
			pMappedData->perspectiveValues.x = 1.0f / proj._11;
			pMappedData->perspectiveValues.y = 1.0f / proj._22;
			pMappedData->perspectiveValues.z = proj._43;
			pMappedData->perspectiveValues.w = -proj._33;
			pMappedData->viewInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_pCamera->GetViewMatrix()));
			pBuffer->Unmap();
			Graphics::SetConstantBuffer(0, eShaderType::PixelShader, pBuffer);

			// ���⼭���� ���� ������ ���� ó���� �޶����� �Ѵ�.
			{
				// �������� �ƿ� �� �κ��� �ǳ� �������
				// diff�� ����ϴ� �͵� �� ����̴�.

				// ������ �����Ѵٸ� ������ ������ �Ѵ�.
				//for (uint32_t i = 0; i < static_cast<uint32_t>(m_Lights.size()); ++i)
				{
					// �׸��� ������ ù ��°�̳� �ƴϳĿ� ����
					// DepthStencil�� Blend State�� �޶�����.
					//if (i == 0)
					//	Graphics::SetDepthStencilState(Renderer::GetForwardLightDS(), 0);
					//else if (i == 1)
					//	Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(), NULL, 0xffffffff);

					// ������ ���� ������ GPU�� �����Ѵ�. Constant Buffer�� ���̴�.
					// �׷��� ���� DirLight�� ������ ���Ҵ�. 
					//auto pLight = m_Lights[i];
					//auto pBuffer = Renderer::GetLightPixelShaderBuffer();
					//auto pMappedData = static_cast<LightPixelShaderBuffer*>(pBuffer->Map());
					//DirectX::XMFLOAT3 pos;
					//DirectX::XMStoreFloat3(&pos, pLight->GetGameObject()->GetTransform()->GetPosition());
					//pMappedData->lightPos = pos;
					//pMappedData->lightRange = 1.0f / pLight->GetRange();
					//pMappedData->lightColor = pLight->GetColor();
					//pMappedData->lightDir = pLight->GetDir();
					//pBuffer->Unmap();
					//Graphics::SetConstantBuffer(1, eShaderType::PixelShader, pBuffer);

					Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, static_cast<Texture*>(Renderer::GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)));
					Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, static_cast<Texture*>(Renderer::GetRenderTarget(eRenderTarget::GBuffer_Color_SpecIntensity)));
					Graphics::SetTexture(eTextureUnit::GBuffer_Normal, static_cast<Texture*>(Renderer::GetRenderTarget(eRenderTarget::GBuffer_Normal)));
					Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, static_cast<Texture*>(Renderer::GetRenderTarget(eRenderTarget::GBuffer_SpecPower)));

					// RenderTarget�� �׸��� shader ����
					Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredDirLightVertexShaderVariation());
					Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredDirLightPixelShaderVariation());

					Graphics::SetVertexBuffer(nullptr);
					Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);

					// clean up
					// �� �κ��� �� �ʿ��ϴ�. ������ ���� �𸣰ڴ�.
					{
						Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_Normal, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, nullptr);

						Graphics::SetDepthStencilState(nullptr, 0);
						//Graphics::GetDeviceContext()->OMSetBlendState(NULL, nullptr, 0xffffffff);
					}
				}
			}
			Graphics::SetShaderVariation(eShaderType::VertexShader, nullptr);
			Graphics::SetShaderVariation(eShaderType::PixelShader, nullptr);
		}
    }
}