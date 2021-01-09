#pragma once
#include "D3D11/VertexTypes.h"


namespace Dive
{
	// 적어도 VertexTypes에 정의된 타입 전부를 지원하도록 할 순 없을까?
	// Mesh 자체를 template class로 만들어야 하나?
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh();

		void Clear();
		
		// vertices
		void SetVertices(const std::vector<Vertex_PosTexNorTan>& vertices) { m_vertices = vertices; }
		std::vector<Vertex_PosTexNorTan>& GetVertices() { return m_vertices; }
		unsigned int GetVerticesCount() const { return static_cast<unsigned int>(m_vertices.size()); }

		// indices
		void SetIndices(const std::vector<unsigned int>& indices) { m_indices = indices; }
		std::vector<unsigned int>& GetIndices() { return m_indices; }
		unsigned int GetIndicesCount() const { return static_cast<unsigned int>(m_indices.size()); }

		unsigned int GetMemorySize();

	private:
		std::vector<Vertex_PosTexNorTan> m_vertices;
		std::vector<unsigned int> m_indices;
	};
}