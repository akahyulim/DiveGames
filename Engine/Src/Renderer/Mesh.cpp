#include "DivePch.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsDefs.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/CoreDefs.h"
#include "Resource/Importer/ModelLoader.h"

namespace Dive
{
	Mesh::Mesh()
		: m_pVertexBuffer(nullptr)
		, m_pIndexBuffer(nullptr)
		, m_pGameObject(nullptr)
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
		DV_DELETE(m_pVertexBuffer);

		m_Indices.clear();
		m_Indices.shrink_to_fit();
		DV_DELETE(m_pIndexBuffer);
	}

	void Mesh::GetGeometry(uint32_t vertexOffset, uint32_t vertexCount,  uint32_t indexOffset, uint32_t indexCount,
		std::vector<uint32_t>* indices, std::vector<VertexStatic>* vertices)
	{
		if (vertices)
		{
			DV_CORE_ASSERT(vertexCount != 0);

			const auto vertex_first = m_Vertices.begin() + vertexOffset;
			const auto vertex_last = m_Vertices.begin() + vertexOffset + vertexCount;
			*vertices = std::vector<VertexStatic>(vertex_first, vertex_last);
		}
		if (indices)
		{
			DV_CORE_ASSERT(indexCount != 0);
			
			const auto index_first = m_Indices.begin() + indexOffset;
			const auto index_last = m_Indices.begin() + indexOffset + indexCount;
			*indices = std::vector<uint32_t>(index_first, index_last);
		}
	}

	void Mesh::AddVertices(const std::vector<VertexStatic>& vertices, uint32_t* pOutVertexOffset)
	{
		if (pOutVertexOffset)
		{
			*pOutVertexOffset = static_cast<uint32_t>(m_Vertices.size());
		}

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}

	void Mesh::AddIndices(const std::vector<uint32_t>& indices, uint32_t* pOutIndexOffset)
	{
		if (pOutIndexOffset)
		{
			*pOutIndexOffset = static_cast<uint32_t>(m_Indices.size());
		}

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}

	uint32_t Mesh::GetVertexCount() const 
	{
		return static_cast<uint32_t>(m_Vertices.size());
	}

	uint32_t Mesh::GetIndexCount() const 
	{
		return static_cast<uint32_t>(m_Indices.size());
	}

	void Mesh::CreateGpuBuffers()
	{
		if(!m_Vertices.empty())
			m_pVertexBuffer = VertexBuffer::Create(m_Vertices.data(), sizeof(VertexStatic), (uint32_t)m_Vertices.size());

		if(!m_Indices.empty())
			m_pIndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
	}
}

