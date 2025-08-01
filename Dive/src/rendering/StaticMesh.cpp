#include "stdafx.h"
#include "StaticMesh.h"
#include "graphics/Buffer.h"
#include "graphics/ShaderManager.h"
#include "math/BoundingBox.h"

namespace Dive
{
	StaticMesh::StaticMesh()
		: m_type(eMeshType::Static)
	{
	}

	StaticMesh::~StaticMesh()
	{
		Clear();
	}

	bool StaticMesh::CreateBuffers(ID3D11Device* device)
	{
		assert(device);

		// 정점 버퍼 생성
		{
			if (m_vertices.empty())
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] 정점이 존재하지 않아 실패");
				return false;
			}

			m_vertexBuffer.reset();
			m_vertexBuffer = std::make_shared<Buffer>(
				device,
				eBufferType::VertexBuffer,
				m_vertices.data(),
				static_cast<uint32_t>(sizeof(LitVertex)),
				static_cast<uint32_t>(m_vertices.size()));
			if (!m_vertexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] VertexBuffer 생성 실패");
				m_vertexBuffer.reset();
				return false;
			}
		}

		// 인덱스 버퍼 생성
		if(!m_indices.empty())
		{
			m_indexBuffer.reset();
			m_indexBuffer = std::make_shared<Buffer>(
				device,
				eBufferType::IndexBuffer,
				m_indices.data(),
				static_cast<uint32_t>(sizeof(uint32_t)),
				static_cast<uint32_t>(m_indices.size()));
			if (!m_indexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] IndexBuffer 생성 실패");
				return false;
			}
		}

		return true;
	}

	void StaticMesh::Clear()
	{
		m_boundingBox.reset();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		m_indices.clear();
		m_indices.shrink_to_fit();

		m_vertices.clear();
		m_vertices.shrink_to_fit();
	}

	void StaticMesh::Draw(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);
		assert(m_vertexBuffer);

		deviceContext->IASetPrimitiveTopology(m_primitiveTopology);

		m_vertexBuffer->Bind(deviceContext);

		if (m_indexBuffer)
		{
			m_indexBuffer->Bind(deviceContext);
			deviceContext->DrawIndexed(m_indexBuffer->GetCount(), 0, 0);
		}
		else
			deviceContext->Draw(m_vertexBuffer->GetCount(), 0);
	}

	void StaticMesh::ComputeBouingBox()
	{
		if (m_vertices.empty())
		{
			DV_LOG(StaticMesh, err, "[::ComputeBouingBox] 빈 정점 정보");
			return;
		}

		m_boundingBox = std::make_unique<BoundingBox>(m_vertices);
	}

	void StaticMesh::AddVertices(const std::vector<LitVertex>& vertices, uint32_t* outOffset)
	{
		// 현재 저장되어 있는 개수 리턴
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_vertices.size());

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	}

	void StaticMesh::AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset)
	{
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_indices.size());

		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	}
}
