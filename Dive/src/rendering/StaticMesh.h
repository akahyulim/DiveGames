#pragma once
#include "resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	enum class eMeshType
	{
		None,
		Static,
		Skinned
	};

	// RenderMesh에서 관리하려면 Mesh라는 Base가 필요하다.
	// => 코파일럿에게 물어보니 유니티 ecs의 경우 SkinnedMeshRendrerer는 별도의 시스템과 컴포넌트가 필요하다고 한다.
	class StaticMesh : public Resource
	{
		DV_CLASS(StaticMesh, Resource)

	public:
		StaticMesh();
		~StaticMesh() override;

		virtual bool CreateBuffers();
		virtual void Clear();

		void SetPositions(const std::vector<DirectX::XMFLOAT3>& positions) { m_Positions = positions; }
		void SetNormals(const std::vector<DirectX::XMFLOAT3>& normals) { m_Normals = normals; }
		void SetTexCoords(const std::vector<DirectX::XMFLOAT2>& texCoords) { m_TexCoords = texCoords; }
		void SetTangents(const std::vector<DirectX::XMFLOAT3>& tangents) { m_Tangents = tangents; }
		void SetColors(const std::vector<DirectX::XMFLOAT4>& colors) { m_Colors = colors; }

		void SetIndices(const std::vector<UINT32>& indices) { m_Indices = indices; }

	protected:
		bool createIndexBuffer();

	protected:
		eMeshType m_Type = eMeshType::None;

		std::vector<DirectX::XMFLOAT3> m_Positions;
		std::vector<DirectX::XMFLOAT3> m_Normals;
		std::vector<DirectX::XMFLOAT2> m_TexCoords;
		std::vector<DirectX::XMFLOAT3> m_Tangents;
		std::vector<DirectX::XMFLOAT4> m_Colors;

		std::vector<UINT32> m_Indices;

		VertexBuffer* m_VertexBuffer = nullptr;
		IndexBuffer* m_IndexBuffer = nullptr;
	};
}