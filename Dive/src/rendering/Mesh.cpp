#include "stdafx.h"
#include "Mesh.h"
#include "core/CoreDefs.h"
#include "graphics/Graphics.h"
#include "graphics/Graphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

namespace Dive
{
	Mesh::Mesh()
		: m_VertexBuffer(nullptr)
		, m_IndexBuffer(nullptr)
		, m_GameObject(nullptr)
	{
	}

	Mesh::~Mesh()
	{
		Clear();
	}

	void Mesh::Clear()
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
		m_VertexBuffer.reset();

		m_Indices.clear();
		m_Indices.shrink_to_fit();
		m_IndexBuffer.reset();
	}

	void Mesh::GetGeometry(UINT32 vertexOffset, UINT32 vertexCount,  UINT32 indexOffset, UINT32 indexCount,
		std::vector<UINT32>* indices, std::vector<VertexStatic>* vertices)
	{
		if (vertices)
		{
			DV_ASSERT(Mesh, vertexCount != 0);

			const auto vertex_first = m_Vertices.begin() + vertexOffset;
			const auto vertex_last = m_Vertices.begin() + vertexOffset + vertexCount;
			*vertices = std::vector<VertexStatic>(vertex_first, vertex_last);
		}
		if (indices)
		{
			DV_ASSERT(Mesh, indexCount != 0);
			
			const auto index_first = m_Indices.begin() + indexOffset;
			const auto index_last = m_Indices.begin() + indexOffset + indexCount;
			*indices = std::vector<UINT32>(index_first, index_last);
		}
	}

	void Mesh::AddVertices(const std::vector<VertexStatic>& vertices, UINT32* outVertexOffset)
	{
		// ���� ����Ǿ� �ִ� ������ ���������� �����Ѵ�.
		// 
		if (outVertexOffset)
		{
			*outVertexOffset = static_cast<UINT32>(m_Vertices.size());
		}

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}

	void Mesh::AddIndices(const std::vector<UINT32>& indices, UINT32* outIndexOffset)
	{
		if (outIndexOffset)
		{
			*outIndexOffset = static_cast<UINT32>(m_Indices.size());
		}

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}

	UINT32 Mesh::GetVertexCount() const 
	{
		return static_cast<UINT32>(m_Vertices.size());
	}

	UINT32 Mesh::GetIndexCount() const 
	{
		return static_cast<UINT32>(m_Indices.size());
	}

	void Mesh::CreateBuffers()
	{
		// ���⿡�� ���� ������ ���� ��ü�� ID3D11Device�� �����Ѵٸ�
		// Mehs�� �� ��� �ش� ��ü�� ������ �ִ��Ĵ� ������ ������ �ȴ�.
		// 1. ���� �޽ô� Graphics�� ID3D11Device�� ������ �ʰ� �� �Լ��� �Ű��������� ���޹��� �� �ִ�.
		// 2. ���۸� �ܺο��� �����ϰ� �޽ÿ� ���Խ�Ű�� ����� �ִ�.
		if (!m_Vertices.empty())
			m_VertexBuffer = VertexBuffer::Generate(m_Vertices.data(), sizeof(VertexStatic), static_cast<UINT32>(m_Vertices.size()));

		if (!m_Indices.empty())
			m_IndexBuffer = IndexBuffer::Generate(m_Indices.data(), static_cast<UINT32>(m_Indices.size()));

	}

	void Mesh::ComputeBouingBox()
	{
		DV_ASSERT(Mesh, !m_Vertices.empty());

		m_BoundingBox = BoundingBox(static_cast<VertexStatic*>(m_Vertices.data()), static_cast<UINT32>(m_Vertices.size()));
	}
}

