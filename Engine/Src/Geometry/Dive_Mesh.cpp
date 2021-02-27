#include "DivePch.h"
#include "Dive_Mesh.h"
#include "Core/Log.h"
#include "Graphics/D3D11/VertexBuffer.h"
#include "Graphics/D3D11/IndexBuffer.h"

namespace Dive
{
	Dive_Mesh::Dive_Mesh()
	{
	}

	Dive_Mesh::~Dive_Mesh()
	{
		Clear();
	}
	
	void Dive_Mesh::Clear()
	{
		m_vertices.clear();
		m_vertices.shrink_to_fit();

		m_indices.clear();
		m_indices.shrink_to_fit();
	}
	
	void Dive_Mesh::AppendVertice(const std::vector<DIVE_VERTEX>& vertices, unsigned int * offset)
	{
		if (offset)
		{
			*offset = static_cast<unsigned int>(vertices.size());
		}

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	}
	
	void Dive_Mesh::AppendIndices(const std::vector<unsigned int>& indices, unsigned int * offset)
	{
		if (offset)
		{
			*offset = static_cast<unsigned int>(indices.size());
		}

		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	}

	unsigned int Dive_Mesh::GetVertexCount() const
	{
		return static_cast<unsigned int>(m_vertices.size());
	}
	
	unsigned int Dive_Mesh::GetIndexCount() const
	{
		return static_cast<unsigned int>(m_indices.size());
	}
	
	unsigned int Dive_Mesh::GetTriangleCount() const
	{
		return GetIndexCount() / 3;
	}

	Dive_MeshFilter::Dive_MeshFilter(Context * context)
		: Resource(context)
	{
		m_resourceType = eResourceType::Mesh;	// ���� ����
	}
	
	Dive_MeshFilter::~Dive_MeshFilter()
	{
	}

	bool Dive_MeshFilter::LoadFromFile(const std::string & filepath)
	{
		// �̰� ���� �����̾�� �Ѵ�.
		// �׷��� rigid, skinned ������ ��� ����? ������� �ٸ��� �ϴ°� ���ݵ�...

		return false;
	}

	bool Dive_MeshFilter::SaveToFile(const std::string & filepath)
	{
		// ���� ���� �������� �����Ѵ�.

		return false;
	}

	void Dive_MeshFilter::AppendGeometry(const std::vector<DIVE_VERTEX>& vertices, const std::vector<unsigned int>& indices, unsigned int * vertexOffset, unsigned int * indexOffset)
	{
		if (vertices.empty() || indices.empty())
		{
			CORE_ERROR("�߸��� ���ڸ� ���޹޾ҽ��ϴ�.");
			return;
		}

		m_mesh->AppendVertice(vertices, vertexOffset);
		m_mesh->AppendIndices(indices, indexOffset);
	}

	bool Dive_MeshFilter::createBuffers()
	{
		const auto vertices = m_mesh->GetVertices();
		const auto indices = m_mesh->GetIndices();

		if (vertices.empty() || indices.empty())
		{
			CORE_ERROR("");
			return false;
		}

		m_vertexBuffer = std::make_shared<VertexBuffer>(m_context);
		if (!m_vertexBuffer->Create<DIVE_VERTEX>(vertices))
		{
			CORE_ERROR("");
			return false;
		}

		m_indexBuffer = std::make_shared<IndexBuffer>(m_context);
		if (!m_indexBuffer->Create(indices))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}