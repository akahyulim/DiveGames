#include "DivePch.h"
#include "SkinnedMesh.h"
#include "IO/Log.h"

namespace Dive
{
	SkinnedMesh::SkinnedMesh()
	{
	}

	SkinnedMesh::~SkinnedMesh()
	{
	}
	
	void SkinnedMesh::AppendVertices(const std::vector<VertexDynamic>& vertices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(vertices.size());

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}
	
	void SkinnedMesh::AppendIndices(const std::vector<uint32_t>& indices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(indices.size());

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}
}