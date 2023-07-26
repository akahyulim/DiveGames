#include "DivePch.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	IMeshRenderer::IMeshRenderer(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_pMaterial = ResourceCache::GetResourceByPath<Material>("Assets/Materials/Default.yaml");
	}

	IMeshRenderer::~IMeshRenderer()
	{
	}

	MeshRenderer::MeshRenderer(GameObject* pGameObject)
		: IMeshRenderer(pGameObject),
		m_pMesh(nullptr)
	{
	}

	MeshRenderer::~MeshRenderer()
	{
		DV_CORE_TRACE("Destroy MeshRenderer: {:s}", GetName());
	}

	// 매개변수로 primitive topology 정도는 받아도 될 것 같다.
	void MeshRenderer::Draw() const
	{
		// Material
		{
			Graphics::SetTexture(eTextureUnit::Albedo, m_pMaterial->GetTexture(eTextureUnit::Albedo));

			// material pixel shader buffer
			{
				auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
				auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
				pMappedData->albedoColor = m_pMaterial->GetAlbedoColor();
				pBuffer->Unmap();
				Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
			}

			// 위치는 여기가 맞지만 좀 애매하다.
			// 그리고 InputLayout 문제가 남아있다.
			Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetBasicVertexShaderVariation());
			Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetBasicPixelShaderVariation());
		}
 
		// Mesh Draw
		{
			auto pBuffer = Renderer::GetModelVertexShaderBuffer();
			auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
			pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetMatrix());
			pBuffer->Unmap();
			Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);

			Graphics::SetVertexBuffer(m_pMesh->GetVertexBuffer());
			Graphics::SetIndexBuffer(m_pMesh->GetIndexBuffer());
			Graphics::DrawIndexed(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				m_pMesh->GetNumIndices(),
				0, 0);
		}
	}

	IMesh* MeshRenderer::GetMesh()
	{
		return m_pMesh ? static_cast<IMesh*>(m_pMesh) : nullptr;
	}

	void MeshRenderer::SetMesh(IMesh* pMesh)
	{
		m_pMesh = dynamic_cast<DvStaticMesh*>(pMesh);
	}
}