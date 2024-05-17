#include "divepch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Core/CoreDefs.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Math/Math.h"

namespace Dive
{
	Renderable::Renderable(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pMaterial(nullptr)
		, m_pMesh(nullptr)
		, m_VertexOffset(0)
		, m_VertexCount(0)
		, m_IndexOffset(0)
		, m_IndexCount(0)
	{
		DirectX::XMStoreFloat4x4(&m_LastTransform, DirectX::XMMatrixIdentity());

		ZeroMemory(&m_CBufferVS, sizeof(VSConstBuf_Model));
		ZeroMemory(&m_CBufferPS, sizeof(PSConstBuf_Model));
	}
	
	Renderable::~Renderable()
	{
		DV_ENGINE_TRACE("컴포넌트({0:s}'s {1:s}) 소멸", GetName(), GetTypeName());
	}

	void Renderable::Update()
	{
		m_CBufferVS.world = DirectX::XMMatrixTranspose(m_pGameObject->GetMatrix());

		// 없다면 초기화된 값을 전달해야 한다.
		if (!m_pMaterial)
			return;

		m_CBufferPS.diffuseColor = m_pMaterial->GetDiffuseColor();
		m_CBufferPS.properties = 0;
		m_CBufferPS.properties |= m_pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;
		m_CBufferPS.properties |= m_pMaterial->HasTexture(eTextureUnit::Normal) ? (1U << 1) : 0;
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

	void Renderable::Draw(D3D11_PRIMITIVE_TOPOLOGY topology) const
	{
		if (GetIndexBuffer() && GetIndexCount() > 0)
		{
			Graphics::GetInstance()->SetVertexBuffer(GetVertexBuffer());
			Graphics::GetInstance()->SetIndexBuffer(GetIndexBuffer());
			Graphics::GetInstance()->DrawIndexed(topology, m_IndexCount, m_IndexOffset);
		}
		else if (GetVertexCount() > 0)
		{
			Graphics::GetInstance()->SetVertexBuffer(GetVertexBuffer());
			Graphics::GetInstance()->Draw(topology, m_VertexCount, m_VertexOffset);
		}
	}
}