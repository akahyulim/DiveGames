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

	void MeshRenderer::Draw()
	{
		// 객체화를 생각한다면 Material과 Mesh에 각각 Draw 함수가 존재하는 것이 맞지 않나 싶다.
		// 실제로 스파키의 경우 Mesh가 Renderable을 상속한 클래스인데
		// Render에서 Material, Vertex & Index Buffer의 Bind 후 Draw
		// 그리고 역순으로 Unbind를 수행한다.
		// 스파키의 경우 MeshComponent는 단순히 Mesh를 가진다. 거의 모든 Component가 이런 형태다...
		// 이왕 스파키를 참고할거면 좀 더 분석해본 후 적용여부를 판단하자.
		
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
				pMappedData->properties = 0; 
				pMappedData->properties  |= m_pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;
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