#pragma once
#include "resource/Resource.h"
#include "math/BoundingBox.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// structures
	//-------------------------------------------------------------------------------------
	struct VertexStatic
	{
		VertexStatic() = default;
		VertexStatic(
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMFLOAT3& bitangent)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->bitangent[0] = bitangent.x;
			this->bitangent[1] = bitangent.y;
			this->bitangent[2] = bitangent.z;
		}

		float position[3]{};
		float texCoords[2]{};
		float normal[3]{};
		float tangent[3]{};
		float bitangent[3]{};
	};

	struct VertexSkinned
	{
		VertexSkinned() = default;
		VertexSkinned(
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMUINT4& boneIDs,
			const DirectX::XMFLOAT4& weights)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->boneIDs[0] = boneIDs.x;
			this->boneIDs[1] = boneIDs.y;
			this->boneIDs[2] = boneIDs.z;
			this->boneIDs[3] = boneIDs.w;

			this->weights[0] = weights.x;
			this->weights[1] = weights.y;
			this->weights[2] = weights.z;
			this->weights[3] = weights.w;
		}

		float position[3]{};
		float texCoords[2]{};
		float normal[3]{};
		float tangent[3]{};
		UINT32 boneIDs[4]{};
		float weights[4]{};
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class VertexBuffer;
	class IndexBuffer;
    struct VertexStatic;

	class Mesh : public Resource
	{
		DV_CLASS(Mesh, Resource)

    public:
        Mesh();
        ~Mesh() override;

        void Clear();

        // 현재 사용하는 곳이 없다.
        // 메시에서 특정 부분의 정점, 인덱스를 리턴
        void GetGeometry(
            UINT32 vertexOffset,
            UINT32 vertexCount,
            UINT32 indexOffset,
            UINT32 indexCount,
            std::vector<UINT32>* indices,
            std::vector<VertexStatic>* vertices
        );

        void AddVertices(const std::vector<VertexStatic>& vertices, UINT32* outVertexOffset = nullptr);
        void AddIndices(const std::vector<UINT32>& indices, UINT32* outIndexOffset = nullptr);

		std::vector<VertexStatic>& GetVertices() { return m_Vertices; }
        std::vector<UINT32>& GetIndices() { return m_Indices; }

        UINT32 GetVertexCount() const;
        UINT32 GetIndexCount() const;

        void CreateBuffers();
		VertexBuffer* GetVertexBuffer() { return m_VertexBuffer; }
        IndexBuffer* GetIndexBuffer() { return m_IndexBuffer; }
        
        void ComputeBouingBox();
        const BoundingBox& GetBoundingBox() const { return m_BoundingBox; }

    private:
        // 1. BonuingBox를 만들려면 적어도 정점 정보는 가지고 있어야 하는 것이 맞다.
        // 2. VertexSkinned를 관리하려면 상속 클래스를 만드는 편이 나아보인다. 
        std::vector<VertexStatic> m_Vertices;
        std::vector<UINT32> m_Indices;

        // urho의 구현을 자세히 설명하자면
        // importer에서 정보(크기, 타입, 데이타)를 파싱한 후 버퍼 객체에 저장만 하고
        // Model의 Save에서 이 정보들을 파일화한다.
        // 그리고 Model의 Load, Clone에서 이 정보들을 이용해 버퍼를 생성하는 것이다.
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;

        BoundingBox m_BoundingBox;
	};
}
