#pragma once
#include "DivePch.h"
#include "VertexTypes.h"

// 아무리 봐도 타입 종속적일 수 밖에 없다.
// 그게 아니라면 단순한 선을 표현하는 Mesh의 Vertex에도 쓰레기값이나 직접 계산한 값을 넣어야 한다.
// 그리고 Rigid와 Skinned의 구분도 모호해지게 된다.
namespace Dive
{
	struct Dive_Mesh
	{
		Dive_Mesh() {}

		unsigned int GetVertexCount() const { return static_cast<unsigned int>(positions.size()); }

		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT4> colors;
		std::vector<DirectX::XMFLOAT2> texCoord;	// 총 4개까지...
		std::vector<DirectX::XMFLOAT3> nomals;
		std::vector<DirectX::XMFLOAT3> tangents;
		// bone weight

		std::vector<unsigned int> indices;
	};

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