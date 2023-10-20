#include "DivePch.h"
#include "SkinnedMesh.h"
#include "Core/Log.h"

namespace Dive
{
	SkinnedMesh::SkinnedMesh(const std::string& name, std::vector<VertexSkinned>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_Name = name;
	}

	SkinnedMesh::SkinnedMesh(const std::string& nodeName, const std::string& name, std::vector<VertexSkinned>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_NodeName = nodeName;
		m_Name = name.empty() ? nodeName : name;
	}

	SkinnedMesh::~SkinnedMesh()
	{
	}

	void SkinnedMesh::CreateVertexBuffer()
	{
		m_pVertexBuffer = new VertexBuffer;
		if (!m_pVertexBuffer->Create<VertexSkinned>(m_Vertices))
		{
			DV_CORE_ERROR("SkinnedMesh::CreateVertexBuffer: VertexBuffer 생성에 실패하였습니다.");
			return;
		}
	}

	void SkinnedMesh::CreateIndexBuffer()
	{
		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
		{
			DV_CORE_ERROR("SkinnedMesh::CreateIndexBuffer: IndexBuffer 생성에 실패하였습니다.");
			return;
		}
	}
}