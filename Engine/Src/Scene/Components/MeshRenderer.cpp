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
		// ��üȭ�� �����Ѵٸ� Material�� Mesh�� ���� Draw �Լ��� �����ϴ� ���� ���� �ʳ� �ʹ�.
		// ������ ����Ű�� ��� Mesh�� Renderable�� ����� Ŭ�����ε�
		// Render���� Material, Vertex & Index Buffer�� Bind �� Draw
		// �׸��� �������� Unbind�� �����Ѵ�.
		// ����Ű�� ��� MeshComponent�� �ܼ��� Mesh�� ������. ���� ��� Component�� �̷� ���´�...
		// �̿� ����Ű�� �����ҰŸ� �� �� �м��غ� �� ���뿩�θ� �Ǵ�����.
		
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

			// ��ġ�� ���Ⱑ ������ �� �ָ��ϴ�.
			// �׸��� InputLayout ������ �����ִ�.
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