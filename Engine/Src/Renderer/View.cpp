#include "DivePch.h"
#include "View.h"
#include "Model.h"
#include "Material.h"
#include "Renderer.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/GBuffer.h"
#include "Graphics/ConstantBuffer.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Drawable.h"
#include "Scene/Components/Light.h"
#include "IO/Log.h"

namespace Dive
{
	View::View(eRenderPath path)
		: m_pCamera(nullptr),
		m_RenderPath(path), 
		m_pGBuffer(nullptr)
	{
	}

	View::View(Camera* pCamera, eRenderPath path)
		: m_pCamera(pCamera),
		m_RenderPath(path),
		m_pGBuffer(nullptr)
	{
	}

	View::~View()
	{
		DV_DELETE(m_pGBuffer);

		DV_CORE_TRACE("{:s} 소멸", m_Name );
	}

	void View::Update(float delta)
	{
		m_Drawables.clear();
		m_Lights.clear();

		const auto& allGameObjects = Scene::GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			if (pGameObject->HasComponent<Drawable>())
				m_Drawables.emplace_back(pGameObject->GetComponent<Drawable>());
			else if (pGameObject->HasComponent<Light>())
				m_Lights.emplace_back(pGameObject->GetComponent<Light>());
		}
	}

	// 기존에는 command별로 알아서 그렸다. 그리고 그 구현을 지향해야 한다.
	// view마다 path가 다를 수 있기 때문이다.
	void View::Render()
	{
		if (m_RenderPath == eRenderPath::Forward)
			renderPathForward();
		else
			renderPathDeferred();
	}

	void View::renderPathForward()
	{
		// Clear
		{
			auto* pRTV = Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Graphics::GetDefaultDepthStencilView();

			Graphics::SetRenderTargetView(0, pRTV);
			Graphics::SetDepthStencilView(pDSV);

			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth, DirectX::XMFLOAT4(0.75f, 0.75f, 1.0f, 1.0f), 1.0f, 0);
		}

		// Draw Model
		{
			if (m_pCamera)
			{
				for(int i = 0; i != static_cast<int>(m_Lights.size()); ++i)
				{
					auto pLight = m_Lights[i];

					DirectX::XMFLOAT3 pos;

					// camera pixel shader buffer
					{
						auto pBuffer = Renderer::GetCameraPixelShaderBuffer();
						auto pMappedData = static_cast<CameraPixelShaderBuffer*>(pBuffer->Map());	
						DirectX::XMStoreFloat3(&pos, m_pCamera->GetGameObject()->GetTransform()->GetPosition());
						pMappedData->cameraPos = pos;
						DirectX::XMFLOAT4X4 proj;
						DirectX::XMStoreFloat4x4(&proj, m_pCamera->GetProjectionMatrix());
						pMappedData->perspectiveValues.x = 1.0f / proj._11;
						pMappedData->perspectiveValues.y = 1.0f / proj._22;
						pMappedData->perspectiveValues.z = proj._43;
						pMappedData->perspectiveValues.w = -proj._33;
						pMappedData->viewInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_pCamera->GetViewMatrix()));
						pBuffer->Unmap();
						Graphics::SetConstantBuffer(0, eShaderType::PixelShader, pBuffer);
					}

					// light pixel shader buffer
					{
						auto pBuffer = Renderer::GetLightPixelShaderBuffer();
						auto pMappedData = static_cast<LightPixelShaderBuffer*>(pBuffer->Map());
						DirectX::XMStoreFloat3(&pos, pLight->GetGameObject()->GetTransform()->GetPosition());
						pMappedData->lightPos = pos;
						pMappedData->lightRange = 1.0f / pLight->GetRange();
						pMappedData->lightColor = pLight->GetColor();
						pMappedData->lightDir = pLight->GetDir();
						pBuffer->Unmap();
						Graphics::SetConstantBuffer(1, eShaderType::PixelShader, pBuffer);
					}

					// out merge
					if (i == 0)
					{
						//Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetForwardLightDS(), 0);
						Graphics::SetDepthStencilState(Renderer::GetForwardLightDS(), 0);
					}
					else if (i == 1)
					{
						Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(), NULL, 0xffffffff);
					}

					// pixel shader
					if (pLight->GetType() == eLightType::Directional)
						Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDirectionalLightPixelShaderVariation());
					else if (pLight->GetType() == eLightType::Point)
						Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetPointLightPixelShaderVariation());

					for (auto pDrawable : m_Drawables)
					{
						{
							Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState());
						
							D3D11_VIEWPORT viewport;
							viewport.Width = 800;
							viewport.Height = 600;
							viewport.MinDepth = 0.0f;
							viewport.MaxDepth = 1.0f;
							viewport.TopLeftX = 0.0f;
							viewport.TopLeftY = 0.0f;
							Graphics::GetDeviceContext()->RSSetViewports(1, &viewport);
						}

						if (pDrawable->HasMaterial())
						{
							auto pMaterial = pDrawable->GetMaterial();
							Graphics::SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));

							// material pixel shader buffer
							{
								auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
								auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
								pMappedData->diffColor = pMaterial->GetColorAlbedo();
								pBuffer->Unmap();
								Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
							}
						}

						// camera vertex shader buffer
						{
							auto pBuffer = Renderer::GetCameraVertexShaderBuffer();
							auto pMappedData = static_cast<CameraVertexShaderBuffer*>(pBuffer->Map());
							pMappedData->viewMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
							pMappedData->projMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
							pBuffer->Unmap();
							Graphics::SetConstantBuffer(0, eShaderType::VertexShader, pBuffer);
						}

						// model vertex shader buffer
						{
							auto pBuffer = Renderer::GetModelVertexShaderBuffer();
							auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
							pMappedData->worldMatrix = DirectX::XMMatrixTranspose(pDrawable->GetGameObject()->GetTransform()->GetMatrix());
							pBuffer->Unmap();
							Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);
						}

						Graphics::SetVertexBuffer(pDrawable->GetModel()->GetVertexBuffer());
						Graphics::SetIndexBuffer(pDrawable->GetModel()->GetIndexBuffer());

						Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetForwardLightVertexShaderVariation());

						Graphics::DrawIndexed(
							D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
							pDrawable->GetMeshIndexCount(),
							pDrawable->GetMeshIndexOffset(),
							pDrawable->GetMeshVertexOffset());
					}
				}
			}
		}

		Graphics::SetDepthStencilState(nullptr, 0);
		Graphics::GetDeviceContext()->OMSetBlendState(NULL, nullptr, 0xffffffff);
	}

	void View::renderPathDeferred()
	{
		if (!m_pGBuffer)
		{
			m_pGBuffer = new GBuffer;
			m_pGBuffer->Create(Graphics::GetWidth(), Graphics::GetWidth());
		}

		// 첫 번째 Draw: GBuffer의 RenderTarget에 opaque object를 그린다.
		{
			// gbuffer 초기화 및 바인드
			m_pGBuffer->PreRender();

			if (m_pCamera)
			{
				for (auto pDrawable : m_Drawables)
				{
					{
						Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState());

						D3D11_VIEWPORT viewport;
						viewport.Width = 800;
						viewport.Height = 600;
						viewport.MinDepth = 0.0f;
						viewport.MaxDepth = 1.0f;
						viewport.TopLeftX = 0.0f;
						viewport.TopLeftY = 0.0f;
						Graphics::GetDeviceContext()->RSSetViewports(1, &viewport);
					}

					// camera vertex shader buffer
					{
						auto pBuffer = Renderer::GetCameraVertexShaderBuffer();
						auto pMappedData = static_cast<CameraVertexShaderBuffer*>(pBuffer->Map());
						pMappedData->viewMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
						pMappedData->projMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
						pBuffer->Unmap();
						Graphics::SetConstantBuffer(0, eShaderType::VertexShader, pBuffer);
					}

					// MeshRenderer에서 처리할 수 있는 사항들
					{
						if (pDrawable->HasMaterial())
						{
							auto pMaterial = pDrawable->GetMaterial();
							Graphics::SetTexture(eTextureUnit::Diffuse, pMaterial->GetTexture(eTextureUnit::Diffuse));

							// material pixel shader buffer
							{
								auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
								auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
								pMappedData->diffColor = pMaterial->GetColorAlbedo();
								pBuffer->Unmap();
								Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
							}
						}

						// model vertex shader buffer
						{
							auto pBuffer = Renderer::GetModelVertexShaderBuffer();
							auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
							pMappedData->worldMatrix = DirectX::XMMatrixTranspose(pDrawable->GetGameObject()->GetTransform()->GetMatrix());
							pBuffer->Unmap();
							Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);
						}

						Graphics::SetVertexBuffer(pDrawable->GetModel()->GetVertexBuffer());
						Graphics::SetIndexBuffer(pDrawable->GetModel()->GetIndexBuffer());

						Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredShadingVertexShaderVariation());
						Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredShadingPixelShaderVariation());

						Graphics::DrawIndexed(
							D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
							pDrawable->GetMeshIndexCount(),
							pDrawable->GetMeshIndexOffset(),
							pDrawable->GetMeshVertexOffset());
					}
				}
			}
			
			// render target bind reset
			m_pGBuffer->PostRender();
		}

		// 두 번째 Draw: Light cb와 GBuffer의 srv를 이용해 광원적용을 계산하여 그린다.
		{
			auto* pRTV = Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Graphics::GetDefaultDepthStencilView();
			Graphics::SetRenderTargetView(0, pRTV);
			Graphics::SetDepthStencilView(pDSV);
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);

			// camera pixel shader buffer
			{
				DirectX::XMFLOAT3 pos;
				auto pBuffer = Renderer::GetCameraPixelShaderBuffer();
				auto pMappedData = static_cast<CameraPixelShaderBuffer*>(pBuffer->Map());
				DirectX::XMStoreFloat3(&pos, m_pCamera->GetGameObject()->GetTransform()->GetPosition());
				pMappedData->cameraPos = pos;
				DirectX::XMFLOAT4X4 proj;
				DirectX::XMStoreFloat4x4(&proj, m_pCamera->GetProjectionMatrix());
				pMappedData->perspectiveValues.x = 1.0f / proj._11;
				pMappedData->perspectiveValues.y = 1.0f / proj._22;
				pMappedData->perspectiveValues.z = proj._43;
				pMappedData->perspectiveValues.w = -proj._33;
				pMappedData->viewInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_pCamera->GetViewMatrix()));
				pBuffer->Unmap();
				Graphics::SetConstantBuffer(0, eShaderType::PixelShader, pBuffer);
			}

			if (!m_Lights.empty())
			{
				
				for (uint32_t i = 0; i < static_cast<uint32_t>(m_Lights.size()); ++i)
				{
					if (i == 0)
					{
						Graphics::SetDepthStencilState(Renderer::GetForwardLightDS(), 0);
					}
					else if (i == 1)
					{
						Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(), NULL, 0xffffffff);
					}

					// light pixel shader buffer
					auto pLight = m_Lights[i];
					auto pBuffer = Renderer::GetLightPixelShaderBuffer();
					auto pMappedData = static_cast<LightPixelShaderBuffer*>(pBuffer->Map());
					DirectX::XMFLOAT3 pos;
					DirectX::XMStoreFloat3(&pos, pLight->GetGameObject()->GetTransform()->GetPosition());
					pMappedData->lightPos = pos;
					pMappedData->lightRange = 1.0f / pLight->GetRange();
					pMappedData->lightColor = pLight->GetColor();
					pMappedData->lightDir = pLight->GetDir();
					pBuffer->Unmap();
					Graphics::SetConstantBuffer(1, eShaderType::PixelShader, pBuffer);
					
					Graphics::SetTexture(eTextureUnit::DepthTex, static_cast<Texture*>(m_pGBuffer->GetDepthStencilTexture()));
					Graphics::SetTexture(eTextureUnit::ColorSpecIntTex, static_cast<Texture*>(m_pGBuffer->GetColorSpecIntTexture()));
					Graphics::SetTexture(eTextureUnit::NormalTex, static_cast<Texture*>(m_pGBuffer->GetNormalTexture()));
					Graphics::SetTexture(eTextureUnit::SpecPowTex, static_cast<Texture*>(m_pGBuffer->GetSpecPowTexture()));

					Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredDirLightVertexShaderVariation());
					Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredDirLightPixelShaderVariation());
					Graphics::SetVertexBuffer(nullptr);
					Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);

					// clean up
					{
						Graphics::SetTexture(eTextureUnit::DepthTex, nullptr);
						Graphics::SetTexture(eTextureUnit::ColorSpecIntTex, nullptr);
						Graphics::SetTexture(eTextureUnit::NormalTex, nullptr);
						Graphics::SetTexture(eTextureUnit::SpecPowTex, nullptr);

						Graphics::SetDepthStencilState(nullptr, 0);
						Graphics::GetDeviceContext()->OMSetBlendState(NULL, nullptr, 0xffffffff);
					}
				}
			}
			
			Graphics::SetShaderVariation(eShaderType::VertexShader, nullptr);
			Graphics::SetShaderVariation(eShaderType::PixelShader, nullptr);
		}
	}
}