#include "DivePch.h"
#include "Mesh.h"
#include "IO/Log.h"

namespace Dive
{
	StaticMesh::StaticMesh()
	{
	}

	StaticMesh::~StaticMesh()
	{
	}
	
	void StaticMesh::AppendVertices(const std::vector<VertexStatic>& vertices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(vertices.size());

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}
	
	void StaticMesh::AppendIndices(const std::vector<uint32_t>& indices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(indices.size());

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}
};

