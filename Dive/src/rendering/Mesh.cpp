﻿#include "stdafx.h"
#include "Mesh.h"
#include "core/CoreDefs.h"
#include "graphics/Graphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

namespace Dive
{
	Mesh::Mesh()
	{
		SetName("Mesh");
	}

	Mesh::~Mesh()
	{
		Clear();
	}

	void Mesh::Clear()
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
		DV_DELETE(m_VertexBuffer);	

		m_Indices.clear();
		m_Indices.shrink_to_fit();
		DV_DELETE(m_IndexBuffer);
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
		// 현재 저장되어 있는 개수를 오프셋으로 리턴한다.
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

