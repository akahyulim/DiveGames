#include "stdafx.h"
#include "StaticMesh.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

namespace Dive
{
	StaticMesh::StaticMesh()
	{
		m_Type = eMeshType::Static;
	}

	StaticMesh::~StaticMesh()
	{
		DV_DELETE(m_VertexBuffer);
		DV_DELETE(m_IndexBuffer);
	}

	bool StaticMesh::CreateBuffers()
	{
		// vertex buffer 생성
		{
			if (m_Positions.empty())
			{
				DV_LOG(StaticMesh, err, "정적 메시 정점이 비어있음");
				return false;
			}
			if (m_Positions.size() != m_Normals.size() ||
				m_Positions.size() != m_Colors.size() ||
				m_Positions.size() != m_Tangents.size() ||
				m_Positions.size() != m_Colors.size())
			{
				DV_LOG(StaticMesh, err, "정적 구성요소의 크기가 일치하지 않음");
				return false;
			}

			DV_DELETE(m_VertexBuffer);

			struct StaticVertex
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT3 normal;
				DirectX::XMFLOAT2 texCoords;
				DirectX::XMFLOAT3 tangent;
				DirectX::XMFLOAT4 color;
			};
			std::vector<StaticVertex> vertices;
			vertices.reserve(m_Positions.size());

			for (size_t i = 0; i != m_Positions.size(); ++i)
			{
				vertices[i] = {
					m_Positions[i],
					m_Normals[i],
					m_TexCoords[i],
					m_Tangents[i],
					m_Colors[i] };
			}

			m_VertexBuffer = new VertexBuffer;
			if (!m_VertexBuffer->Create(
				vertices.data(),
				sizeof(StaticVertex),
				static_cast<UINT32>(vertices.size())))
			{
				DV_LOG(StaticMesh, err, "정적 메시 버텍스 버퍼 생성 실패");
				return false;
			}
		}

		if (!createIndexBuffer())
			return false;

		return true;
	}

	void StaticMesh::Clear()
	{
		m_Colors.clear();
		m_Colors.shrink_to_fit();
		m_Tangents.clear();	
		m_Tangents.shrink_to_fit();
		m_TexCoords.clear();
		m_TexCoords.shrink_to_fit();
		m_Normals.clear();
		m_Normals.shrink_to_fit();
		m_Positions.clear();
		m_Positions.shrink_to_fit();

		m_Indices.clear();
		m_Indices.shrink_to_fit();
	}

	bool StaticMesh::createIndexBuffer()
	{
		if (m_Indices.empty())
		{
			DV_LOG(StaticMesh, err, "메시 인덱스가 비어있음");
			return false;
		}

		DV_DELETE(m_IndexBuffer);

		m_IndexBuffer = new IndexBuffer;
		if (!m_IndexBuffer->Create(
			m_Indices.data(),
			static_cast<UINT32>(m_Indices.size())))
		{
			DV_LOG(StaticMesh, err, "메시 인덱스 버퍼 생성 실패");
			return false;
		}

		return true;
	}
}