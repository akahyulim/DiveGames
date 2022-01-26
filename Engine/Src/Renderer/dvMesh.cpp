#include "divepch.h"
#include "dvMesh.h"
#include "Base/DiveCore.h"

namespace Dive
{
	dvMesh::dvMesh()
		: m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr)
	{
	}

	dvMesh::~dvMesh()
	{
	}

	void dvMesh::SetIndices(const std::vector<uint32_t>& indices)
	{
		if (indices.empty())
			return;

		if (!m_indices.empty())
			m_indices.clear();

		m_indices = indices;
	}
}