#include "DivePch.h"
#include "Mesh.h"
#include "D3D11/VertexTypes.h"


namespace Dive
{
	Mesh::~Mesh()
	{
		Clear();
	}

	void Mesh::Clear()
	{
		if (!m_vertices.empty())
		{
			m_vertices.clear();
			m_vertices.shrink_to_fit();
		}

		if (!m_indices.empty())
		{
			m_indices.clear();
			m_indices.shrink_to_fit();
		}
	}

	unsigned int Mesh::GetMemorySize()
	{
		auto verticesMemorySize = m_vertices.size() * sizeof(Vertex_PosTexNorTan);
		auto indicesMemorySize	= m_indices.size() * sizeof(unsigned int);

		return static_cast<unsigned int>(verticesMemorySize + indicesMemorySize);
	}
}