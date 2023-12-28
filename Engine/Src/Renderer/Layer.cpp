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

// 스파키를 참고
// Layer는 윈도우 객체를 관리하며 초기화함수와 다수의 콜백함수로 구성된다.
// 그리고 구체 Layer에서 해당 함수들을 구현하여 사용한다.
// 구체 Layer는 기본적으로 Scene과 Rendere를 가지며
// 콜백 함수는 일들을 이용해 처리를 수행한다.
// 여기에서 Renderer는 나의 Graphics + Renderer의 기능을 가지는 클래스로
// 바인드함수와 각종 리소스 버퍼(상수 버퍼)들을 관리한다.
namespace Dive
{
    Layer::Layer()
    {
    }

    Layer::~Layer()
    {
        // 단순하게 봤을 땐 레이어당 씬 하나라서 씬을 제거하는 것이 맞다.
        // 하지만 동일한 씬을 다른 레이어에서 그릴 수 도 있다.
    }
    
    void Layer::Update()
    {
		m_pCamera = nullptr;
        m_Renderables.clear();

        auto allGameObjects = Scene::GetAllGameObjects();
        for (auto* pGameObject : allGameObjects)
        {
			// 카메라를 생성자 혹은 다른 함수를 통해 직접 받을 수도 있다.
            if (Camera* pCamera = pGameObject->GetComponent<Camera>())
            {
				// 씬에 카메라가 여러개일 순 있지만 레이어는 하나만 관리하는 게 맞지 않나?
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

	// 현재 Graphics에 랩핑된 함수와 직접 Context를 받아와 바인드 하는 함수가 섞여있다.
	// Graphics가 필요한가라는 생각이 든다. 
	// sparky의 경우 각자의 bind, draw 함수에서 dxcontext로부터 devicecontext를 가져와 직접 호출한다.
	// 그런데 문제는 SetRenderTarget()은 아직 찾지 못했다는거다... => 이건 프로젝트의 단순함때문인 듯 하다.
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
			// 기본적으론 컬백이 맞으나 plane이나 triangle처럼 하나의 면으로 만들어진 메시는 좀 애매하다.
			Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

			// MeshRenderer 그리기
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
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());	// 이건 좀 에바?
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);

			// camera - 기타 정보(광원 계산용)
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

			// 여기서부터 광원 유무에 따라 처리가 달라져야 한다.
			{
				// 기존에는 아에 이 부분을 건너 띄었지만
				// diff만 출력하는 것도 한 방법이다.

				// 광원이 존재한다면 루프를 돌려야 한다.
				//for (uint32_t i = 0; i < static_cast<uint32_t>(m_Lights.size()); ++i)
				{
					// 그리고 광원이 첫 번째이냐 아니냐에 따라
					// DepthStencil과 Blend State가 달라진다.
					//if (i == 0)
					//	Graphics::SetDepthStencilState(Renderer::GetForwardLightDS(), 0);
					//else if (i == 1)
					//	Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(), NULL, 0xffffffff);

					// 광원에 대한 정보를 GPU로 전달한다. Constant Buffer로 말이다.
					// 그런데 현재 DirLight만 구현해 놓았다. 
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

					// RenderTarget에 그리는 shader 전달
					Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredDirLightVertexShaderVariation());
					Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredDirLightPixelShaderVariation());

					Graphics::SetVertexBuffer(nullptr);
					Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);

					// clean up
					// 이 부분이 꼭 필요하다. 이유는 아직 모르겠다.
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