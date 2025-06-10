#include "stdafx.h"
#include "skinnedMesh.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

namespace Dive
{
	SkinnedMesh::SkinnedMesh()
	{
		m_Type = eMeshType::Skinned;
	}

	bool SkinnedMesh::CreateBuffers()
	{
		// vertex buffer 생성
		{
			if (m_Positions.empty())
			{
				DV_LOG(SkinnedMesh, err, "정적 메시 정점이 비어있음");
				return false;
			}
			if (m_Positions.size() != m_Normals.size() ||
				m_Positions.size() != m_TexCoords.size() ||
				m_Positions.size() != m_Tangents.size() ||
				m_Positions.size() != m_Colors.size() ||
				m_Positions.size() != m_BoneWeights.size() ||
				m_Positions.size() != m_BoneIndices.size())
			{
				DV_LOG(SkinnedMesh, err, "정적 구성요소 크기가 일치하지 않음");
				return false;
			}

			DV_DELETE(m_VertexBuffer);

			struct SkinnedVertex
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT3 normal;
				DirectX::XMFLOAT2 texCoords;
				DirectX::XMFLOAT3 tangent;
				DirectX::XMFLOAT4 color;
				DirectX::XMFLOAT4 weights;
				DirectX::XMFLOAT4 boneIndices;
			};
			std::vector<SkinnedVertex> vertices;
			vertices.reserve(m_Positions.size());

			for (size_t i = 0; i != m_Positions.size(); ++i)
			{
				vertices[i] = {
					m_Positions[i],
					m_Normals[i],
					m_TexCoords[i],
					m_Tangents[i],
					m_Colors[i],
					m_BoneWeights[i],
					m_BoneIndices[i] };
			}

			m_VertexBuffer = new VertexBuffer;
			if (!m_VertexBuffer->Create(
				vertices.data(),
				sizeof(SkinnedVertex),
				static_cast<UINT32>(vertices.size())))
			{
				DV_LOG(SkinnedMesh, err, "스킨드 메시 버텍스 버퍼 생성 실패");
				return false;
			}
		}

		if (!createIndexBuffer())
			return false;

		return true;
	}

	void SkinnedMesh::Clear()
	{
		m_BoneIndices.clear();
		m_BoneIndices.shrink_to_fit();
		m_BoneWeights.clear();
		m_BoneWeights.shrink_to_fit();

		StaticMesh::Clear();
	}
}