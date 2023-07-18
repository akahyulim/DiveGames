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

	DvStaticMesh::DvStaticMesh(const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_Name = name;
	}

	DvStaticMesh::DvStaticMesh(const std::string& nodeName, const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_NodeName = nodeName;
		m_Name = name.empty() ? nodeName : name;
	}

	DvStaticMesh::~DvStaticMesh()
	{

	}

	void DvStaticMesh::CreateVertexBuffer()
	{
		m_pVertexBuffer = new VertexBuffer;
		if (!m_pVertexBuffer->Create<VertexStatic>(m_Vertices))
		{
			DV_CORE_ERROR("DvStaticMesh::CreateVertexBuffer: VertexBuffer 생성에 실패하였습니다.");
			return;
		}
	}

	void DvStaticMesh::CreateIndexBuffer()
	{
		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
		{
			DV_CORE_ERROR("DvStaticMesh::CreateIndexBuffer: IndexBuffer 생성에 실패하였습니다.");
			return;
		}
	}
};

