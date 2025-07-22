#include "stdafx.h"
#include "StaticMesh.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/ShaderManager.h"
#include "math/BoundingBox.h"

namespace Dive
{
	StaticMesh::StaticMesh()
		: m_Type(eMeshType::Static)
	{
	}

	StaticMesh::~StaticMesh()
	{
		Clear();
	}

	bool StaticMesh::CreateBuffers()
	{
		if (m_Vertices.empty())
		{
			DV_LOG(StaticMesh, err, "정점 empty");
			return false;
		}

		m_VertexBuffer.reset();

		m_VertexBuffer = std::make_unique<VertexBuffer>();
		if (!m_VertexBuffer->Create(
			m_Vertices.data(),
			sizeof(StaticVertex),
			static_cast<uint32_t>(m_Vertices.size())))
		{
			DV_LOG(StaticMesh, err, "정점 버퍼 생성 실패");
			return false;
		}

		if (!createIndexBuffer())
		{
			DV_LOG(StaticMesh, err, "인덱스 버퍼 생성 실패");
			return false;
		}

		return true;
	}

	void StaticMesh::Clear()
	{
		m_BoundingBox.reset();
		m_IndexBuffer.reset();
		m_VertexBuffer.reset();

		m_Indices.clear();
		m_Indices.shrink_to_fit();

		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
	}

	void StaticMesh::Draw(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);
		assert(m_VertexBuffer);

		deviceContext->IASetPrimitiveTopology(m_PrimitiveTopology);

		ID3D11Buffer* vertexBuffer = m_VertexBuffer->GetBuffer();
		UINT stride = m_VertexBuffer->GetStride();
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

		if (m_IndexBuffer)
		{
			deviceContext->IASetIndexBuffer(m_IndexBuffer->GetBuffer(), m_IndexBuffer->GetFormat(), 0);
			deviceContext->DrawIndexed(m_IndexBuffer->GetCount(), 0, 0);
		}
		else
			deviceContext->Draw(m_VertexBuffer->GetCount(), 0);
	}

	void StaticMesh::ComputeBouingBox()
	{
		if (m_Vertices.empty())
		{
			DV_LOG(StaticMesh, err, "vertices empty");
			return;
		}

		m_BoundingBox = std::make_unique<BoundingBox>(m_Vertices);
	}

	void StaticMesh::AddVertices(const std::vector<StaticVertex>& vertices, uint32_t* outOffset)
	{
		// 현재 저장되어 있는 개수 리턴
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_Vertices.size());

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}

	void StaticMesh::AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset)
	{
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_Indices.size());

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}

	Shader* StaticMesh::GetVertexShader() const
	{
		auto shader = ShaderManager::GetShader(m_VertexShaderName);
		if (!shader || shader->GetType() != eShaderType::Vertex)
		{
			DV_LOG(StaticMesh, err, "정점 셰이더 획득 실패: {}", m_VertexShaderName);
			return nullptr;
		}

		return shader;
	}

	bool StaticMesh::createIndexBuffer()
	{
		if (m_Indices.empty())
		{
			DV_LOG(StaticMesh, err, "indices empty");
			return false;
		}

		m_IndexBuffer.reset();

		m_IndexBuffer = std::make_unique<IndexBuffer>();
		if (!m_IndexBuffer->Create(
			m_Indices.data(),
			static_cast<uint32_t>(m_Indices.size()),
			static_cast<uint32_t>(m_Vertices.size())))
		{
			DV_LOG(StaticMesh, err, "인덱스 버퍼 생성 실패");
			return false;
		}

		return true;
	}
}
