#include "DivePch.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	Mesh::Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
		: Resource(eResourceType::Mesh)
		, m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
	}

	Mesh::Mesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
		: Resource(eResourceType::Mesh)
		, m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
		SetName(name);
	}
	
	Mesh::~Mesh()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);
	}
	
	// 현재 urho3d의 Geometry::Draw와 완전히 같다.
	void Mesh::Draw(D3D11_PRIMITIVE_TOPOLOGY topology)
	{	
		if (m_pIndexBuffer && m_pIndexBuffer->GetCount() > 0)
		{
			Graphics::SetVertexBuffer(m_pVertexBuffer);
			Graphics::SetIndexBuffer(m_pIndexBuffer);
			Graphics::DrawIndexed(topology, m_pIndexBuffer->GetCount(), 0);
		}
		else if (m_pVertexBuffer->GetCount() > 0)
		{
			Graphics::SetVertexBuffer(m_pVertexBuffer);
			Graphics::Draw(topology, m_pVertexBuffer->GetCount(), 0);
		}
	}

	Mesh* Mesh::Create(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
	{
		return new Mesh(name, pVertexBuffer, pIndexBuffer);
	}
};

