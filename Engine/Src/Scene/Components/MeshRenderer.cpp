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
#include "Core/Log.h"

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
	}

	// 현재 렌더러의 업데이트에서 이 함수를 호출하고 있다.
	// 반면 스파키는 씬에서 엔터티로부터 메시를 획득한 후 메시의 렌더 함수를 호출한다.
	// 어찌됐든 둘 다 이 메시렌더러라는 이름과는 어울리지 않는다...
	// 일단 현재 목표는 렌더러에서 레이어를 어떻게 해보는 것이다.
	void MeshRenderer::Draw()
	{
		// Material
		{
			Graphics::SetTexture(eTextureUnit::Diffuse, m_pMaterial->GetTexture(eTextureUnit::Diffuse));

			// material pixel shader buffer
			{
				auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
				auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
				pMappedData->diffuseColor = m_pMaterial->GetDiffuseColor();
				pMappedData->tiling = m_pMaterial->GetTiling();
				pMappedData->offset = m_pMaterial->GetOffset();
				pBuffer->Unmap();
				Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
			}

			// 위치는 여기가 맞지만 좀 애매하다.
			// 그리고 InputLayout 문제가 남아있다.
			Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredShadingVertexShaderVariation());//Renderer::GetBasicVertexShaderVariation());
			Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredShadingPixelShaderVariation());//Renderer::GetBasicPixelShaderVariation());
		}
 
		// Mesh Draw
		{
			auto pBuffer = Renderer::GetModelVertexShaderBuffer();
			auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
			pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetWorldMatrix());
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
		m_pMesh = dynamic_cast<StaticMesh*>(pMesh);
	}
}