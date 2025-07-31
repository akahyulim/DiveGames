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

			m_vb.reset();
			m_vb = std::make_shared<Buffer>(
				device,
				eBufferType::VertexBuffer,
				m_vertices.data(),
				static_cast<uint32_t>(sizeof(LitVertex)),
				static_cast<uint32_t>(m_vertices.size()));
			if (!m_vb)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] VertexBuffer 생성 실패");
				m_vb.reset();
				return false;
			}
		}

		// 인덱스 버퍼 생성
		if(!m_indices.empty())
		{
			m_ib.reset();
			m_ib = std::make_shared<Buffer>(
				device,
				eBufferType::IndexBuffer,
				m_indices.data(),
				static_cast<uint32_t>(sizeof(uint32_t)),
				static_cast<uint32_t>(m_indices.size()));
			if (!m_ib)
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
		m_ib.reset();
		m_vb.reset();

		m_indices.clear();
		m_indices.shrink_to_fit();

		m_vertices.clear();
		m_vertices.shrink_to_fit();
	}

	void StaticMesh::Draw(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);
		assert(m_vb);

		deviceContext->IASetPrimitiveTopology(m_primitiveTopology);

		m_vb->Bind(deviceContext);

		if (m_ib)
		{
			m_ib->Bind(deviceContext);
			deviceContext->DrawIndexed(m_ib->GetCount(), 0, 0);
		}
		else
			deviceContext->Draw(m_vb->GetCount(), 0);
	}

	void StaticMesh::ComputeBouingBox()
	{
		if (m_vertices.empty())
		{
			DV_LOG(StaticMesh, err, "vertices empty");
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
