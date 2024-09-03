#include "divepch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Core/CoreDefs.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/ConstantBuffer.h"
#include "Math/Math.h"

namespace Dive
{
#pragma pack(push, 1)
	struct CB_OBJECT_VS
	{
		DirectX::XMMATRIX world;
	};

	struct CB_MATERIAL_PS
	{
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 normal;

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};
#pragma pack(pop)

	Renderable::Renderable(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pMaterial(nullptr)
		, m_pMesh(nullptr)
		, m_VertexOffset(0)
		, m_VertexCount(0)
		, m_IndexOffset(0)
		, m_IndexCount(0)
		, m_pCBufferVS(nullptr)
		, m_pCBufferPS(nullptr)
	{
		DirectX::XMStoreFloat4x4(&m_LastTransform, DirectX::XMMatrixIdentity());
	}
	
	Renderable::~Renderable()
	{
		DV_DELETE(m_pCBufferPS);
		DV_DELETE(m_pCBufferVS);
	}

	void Renderable::Update()
	{
		// vs constant buffer
		{
			if (!m_pCBufferVS)
				m_pCBufferVS = ConstantBuffer::Create("CB_OBJECT_VS", sizeof(CB_OBJECT_VS));

			auto pMappedData = reinterpret_cast<CB_OBJECT_VS*>(m_pCBufferVS->Map());
			pMappedData->world = DirectX::XMMatrixTranspose(m_pGameObject->GetMatrix());
			m_pCBufferVS->Unmap();
		}
		
		// ps constant buffer
		{
			// 없다면 초기화된 값을 전달해야 한다.
			if (!m_pMaterial)
			{
				m_pMaterial = new Material();
				m_pMaterial->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
				//return;
			}

			if (!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_MATERIAL_PS", sizeof(CB_MATERIAL_PS));
			
			auto pMappedData = reinterpret_cast<CB_MATERIAL_PS*>(m_pCBufferPS->Map());
			pMappedData->diffuse = m_pMaterial->GetDiffuseColor();
			pMappedData->properties = 0;
			pMappedData->properties |= m_pMaterial->HasTexture(eTextureUnitType::Diffuse) ? (1U << 0) : 0;
			pMappedData->properties |= m_pMaterial->HasTexture(eTextureUnitType::Normal) ? (1U << 1) : 0;
			m_pCBufferPS->Unmap();
		}
	}
	
	// 렌더러블이 그릴 메시의 정보를 받는 메서드다.
	// 메시는 단일 정점 버퍼를 사용하기에 오프셋은 사실상 전부 0이다.
	void Renderable::SetGeometry(Mesh* pMesh, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount)
	{
		m_pMesh = pMesh;
		m_BoundingBox = pMesh->GetBoundingBox();
		m_TransformdBoundingBox = pMesh->GetBoundingBox();
		m_VertexOffset = vertexOffset;
		m_VertexCount = vertexCount;
		m_IndexOffset = indexOffset;
		m_IndexCount = indexCount;

		pMesh->SetGameObject(GetGameObject());

		DV_ENGINE_DEBUG("vertex offset: {0:d}, indexOffset: {1:d}", vertexOffset, indexOffset);
	}

	VertexBuffer* Renderable::GetVertexBuffer() const
	{
		if(!m_pMesh)
			return nullptr;

		return m_pMesh->GetVertexBuffer();
	}
	
	IndexBuffer* Renderable::GetIndexBuffer() const
	{
		if(!m_pMesh)
			return nullptr;

		return m_pMesh->GetIndexBuffer();
	}

	const BoundingBox& Renderable::GetBoundingBox()
	{
		const auto currentTransform = m_pGameObject->GetMatrix();
		if (!Math::CompareXMMATRIX(DirectX::XMLoadFloat4x4(&m_LastTransform), currentTransform))
		{
			m_TransformdBoundingBox = m_BoundingBox.Transform(currentTransform);
			DirectX::XMStoreFloat4x4(&m_LastTransform, currentTransform);
		}

		return m_TransformdBoundingBox;
	}

	// 제거대상 - 사용하지 않는다.
	void Renderable::Draw(D3D11_PRIMITIVE_TOPOLOGY topology) const
	{
		if (GetIndexBuffer() && GetIndexCount() > 0)
		{
			Graphics::GetInstance()->SetVertexBuffer(GetVertexBuffer());
			Graphics::GetInstance()->SetIndexBuffer(GetIndexBuffer());
			Graphics::GetInstance()->DrawIndexed(m_IndexCount, m_IndexOffset);
		}
		else if (GetVertexCount() > 0)
		{
			Graphics::GetInstance()->SetVertexBuffer(GetVertexBuffer());
			Graphics::GetInstance()->Draw(m_VertexCount, m_VertexOffset);
		}
	}
}