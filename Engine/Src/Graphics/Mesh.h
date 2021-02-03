#pragma once
#include "DivePch.h"
#include "VertexTypes.h"

// �ƹ��� ���� Ÿ�� �������� �� �ۿ� ����.
// �װ� �ƴ϶�� �ܼ��� ���� ǥ���ϴ� Mesh�� Vertex���� �����Ⱚ�̳� ���� ����� ���� �־�� �Ѵ�.
// �׸��� Rigid�� Skinned�� ���е� ��ȣ������ �ȴ�.
namespace Dive
{
	struct Dive_Mesh
	{
		Dive_Mesh() {}

		unsigned int GetVertexCount() const { return static_cast<unsigned int>(positions.size()); }

		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT4> colors;
		std::vector<DirectX::XMFLOAT2> texCoord;	// �� 4������...
		std::vector<DirectX::XMFLOAT3> nomals;
		std::vector<DirectX::XMFLOAT3> tangents;
		// bone weight

		std::vector<unsigned int> indices;
	};

	// Ÿ�� ������ �ʿ��� �� �ִ�.
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