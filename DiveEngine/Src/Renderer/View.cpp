#include "DivePch.h"
#include "View.h"
#include "Model.h"
#include "Material.h"
#include "Renderer.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "IO/Log.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Drawable.h"
#include "Scene/Components/Transform.h"

namespace Dive
{
	View::View()
		: m_pCamera(nullptr)
	{
	}

	View::View(Camera* pCamera)
		: m_pCamera(pCamera)
	{
	}

	View::~View()
	{
		DV_CORE_TRACE("{:s} 소멸", m_Name );
	}

	void View::Update(float delta)
	{
		m_Drawables.clear();

		const auto& allGameObjects = Scene::GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			if (pGameObject->HasComponent<Drawable>())
				m_Drawables.emplace_back(pGameObject->GetComponent<Drawable>());
		}
	}

	// 기존에는 command별로 알아서 그렸다. 그리고 그 구현을 지향해야 한다.
	// view마다 path가 다를 수 있기 때문이다.
	void View::Render()
	{
		// Clear
		{
			auto* pRTV = Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Graphics::GetDefaultDepthStencilView();

			Graphics::SetRenderTargetViews(0, pRTV);
			Graphics::SetDepthStencilView(pDSV);

			Graphics::ClearRenderTargets(eClearTarget::Color | eClearTarget::Depth, DirectX::XMFLOAT4(0.75f, 0.75f, 1.0f, 1.0f), 1.0f, 0);
		}

		// Draw Model
		{
			if (m_pCamera)
			{
				for (auto pDrawable : m_Drawables)
				{
					{
						Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState(), 1);
						Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState());
					}

					{
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
						Graphics::SetTexture(0, pMaterial->GetTexture(eTextureUnit::Diffuse));
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

					// camera pixel shader buffer
					{
						//auto pBuffer = Renderer::GetCameraPixelShaderBuffer();
						//auto pMappedData = static_cast<CameraPixelShaderBuffer*>(pBuffer->Map());
						//pMappedData->cameraPos = m_pCamera->GetGameObject()->GetTransform()->GetPosition();
						//pBuffer->Unmap();
						//Graphics::SetConstantBuffer(eShaderType::PixelShader, pBuffer);
					}

					// light pixel shader buffer
					{

					}

					Graphics::SetVertexBuffer(pDrawable->GetModel()->GetVertexBuffer());
					Graphics::SetIndexBuffer(pDrawable->GetModel()->GetIndexBuffer());

					Graphics::SetShaderVariation(Renderer::GetVertexShaderVariation());
					Graphics::SetShaderVariation(Renderer::GetPixelShaderVariation());

					Graphics::DrawIndexed(
						D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						pDrawable->GetMeshIndexCount(),
						pDrawable->GetMeshIndexOffset(),
						pDrawable->GetMeshVertexOffset());
				}
			}
		}
	}
}