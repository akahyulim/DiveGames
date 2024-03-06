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
		: m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
	}

	Mesh::Mesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
		: m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
		SetName(name);
	}
	
	Mesh::~Mesh()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);

		DV_CORE_TRACE("resource destroy - {0:s}({1:d}), {2:s}({3:d})",
			GetTypeName(), GetTypeHash(), GetName(), GetNameHash());
	}
	
	// ���� urho3d�� Geometry::Draw�� ������ ����.
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

	Mesh* CreateMesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
	{
		if (!pVertexBuffer)
		{
			DV_CORE_ERROR("���ؽ� ���۰� �������� �ʾ� �޽ø� ������ �� �����ϴ�.");
			return nullptr;
		}

		return new Mesh(name, pVertexBuffer, pIndexBuffer);
	}
};

