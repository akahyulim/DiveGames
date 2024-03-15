#include "divepch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Core/CoreDefs.h"
#include "Renderer/Mesh.h"

#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

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
	}
	
	Renderable::~Renderable()
	{
		DV_CORE_TRACE("ÄÄÆ÷³ÍÆ®({0:s}'s {1:s}) ¼Ò¸ê", GetName(), GetTypeName());
	}
	
	void Renderable::SetGeometry(Mesh* pMesh, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount)
	{
		m_pMesh = pMesh;
		m_VertexOffset = vertexOffset;
		m_VertexCount = vertexCount;
		m_IndexOffset = indexOffset;
		m_IndexCount = indexCount;

		pMesh->SetGameObject(GetGameObject());
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

	void Renderable::Draw(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (GetIndexBuffer() && GetIndexCount() > 0)
		{
			Graphics::SetVertexBuffer(GetVertexBuffer());
			Graphics::SetIndexBuffer(GetIndexBuffer());
			Graphics::DrawIndexed(topology, m_IndexCount, m_IndexOffset);
		}
		else if (GetVertexCount() > 0)
		{
			Graphics::SetVertexBuffer(GetVertexBuffer());
			Graphics::Draw(topology, m_VertexCount, m_VertexOffset);
		}
	}
}