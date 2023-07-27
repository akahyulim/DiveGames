#include "DivePch.h"
#include "Mesh.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	IMesh::IMesh()
		: m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr)
	{
	}

	IMesh::~IMesh()
	{
		DV_DELETE(m_pVertexBuffer);
		DV_DELETE(m_pIndexBuffer);
	}

	StaticMesh::StaticMesh(const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_Name = name;
	}

	StaticMesh::StaticMesh(const std::string& nodeName, const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_NodeName = nodeName;
		m_Name = name.empty() ? nodeName : name;
	}

	StaticMesh::~StaticMesh()
	{

	}

	void StaticMesh::CreateVertexBuffer()
	{
		m_pVertexBuffer = new VertexBuffer;
		if (!m_pVertexBuffer->Create<VertexStatic>(m_Vertices))
		{
			DV_CORE_ERROR("StaticMesh::CreateVertexBuffer: VertexBuffer 생성에 실패하였습니다.");
			return;
		}
	}

	void StaticMesh::CreateIndexBuffer()
	{
		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
		{
			DV_CORE_ERROR("StaticMesh::CreateIndexBuffer: IndexBuffer 생성에 실패하였습니다.");
			return;
		}
	}
};

