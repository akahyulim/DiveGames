#pragma once
#include "DivePch.h"
#include "VertexTypes.h"


namespace Dive
{
	// 타입 리턴이 필요할 수 있다.
	template<typename VertexType, typename IndexType = unsigned int>
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const Mesh<VertexType, typename IndexType>& other)
		{
			m_vertices	= other.m_vertices;
			m_indices	= other.m_indices;
		}
		~Mesh()
		{
			Clear();
		}

		void Clear()
		{
			if (!m_indices.empty())
			{
				m_indices.clear();
				m_indices.shrink_to_fit();
			}

			if (!m_vertices.empty())
			{
				m_vertices.clear();
				m_vertices.shrink_to_fit();
			}
		}

		// vertex
		void SetVertices(const std::vector<VertexType>& vertices)	{ m_vertices = vertices; }
		std::vector<VertexType>& GetVertices()						{ return m_vertices; }
		unsigned int GetVertexCount() const							{ return static_cast<unsigned int>(m_vertices.size()); }

		// index
		void SetIndices(const std::vector<IndexType>& indices)		{ m_indices = indices; }
		std::vector<IndexType>& GetIndices()						{ return m_indices; }
		unsigned int GetIndexCount() const							{ return static_cast<unsigned int>(m_indices.size()); }

	private:
	private:
		std::vector<VertexType> m_vertices;
		std::vector<IndexType> m_indices;
	};
}