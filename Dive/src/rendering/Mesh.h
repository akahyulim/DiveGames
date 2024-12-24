#pragma once
#include "Math/BoundingBox.h"

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
		uint32_t boneIDs[4]{};
		float weights[4]{};
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class VertexBuffer;
	class IndexBuffer;
    struct VertexStatic;
    class GameObject;

	class Mesh
	{
    public:
        Mesh();
        ~Mesh();

        void Clear();

        // ���� ����ϴ� ���� ����.
        // �޽ÿ��� Ư�� �κ��� ����, �ε����� ����
        void GetGeometry(
            uint32_t vertexOffset,
            uint32_t vertexCount,
            uint32_t indexOffset,
            uint32_t indexCount,
            std::vector<uint32_t>* indices,
            std::vector<VertexStatic>* vertices
        );

        void AddVertices(const std::vector<VertexStatic>& vertices, uint32_t* pOutVertexOffset = nullptr);
        void AddIndices(const std::vector<uint32_t>& indices, uint32_t* pOutIndexOffset = nullptr);

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        std::vector<VertexStatic>& GetVertices() { return m_Vertices; }
        std::vector<uint32_t>& GetIndices() { return m_Indices; }

        uint32_t GetVertexCount() const;
        uint32_t GetIndexCount() const;

        void CreateBuffers();
        IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }
        VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }

        void ComputeBouingBox();
        const BoundingBox& GetBoundingBox() const { return m_BoundingBox; }

        // mesh�κ��� gameObject�� renderer���� ������ �� �ִ�.
        GameObject* GetGameObject() const { return m_pGameObject; }
        void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }

    private:
        std::string m_Name;

        // 1. BonuingBox�� ������� ��� ���� ������ ������ �־�� �ϴ� ���� �´�.
        // 2. VertexSkinned�� �����Ϸ��� ��� Ŭ������ ����� ���� ���ƺ��δ�. 
        std::vector<VertexStatic> m_Vertices;
        std::vector<uint32_t> m_Indices;

        // urho�� ������ �ڼ��� �������ڸ�
        // importer���� ����(ũ��, Ÿ��, ����Ÿ)�� �Ľ��� �� ���� ��ü�� ���常 �ϰ�
        // Model�� Save���� �� �������� ����ȭ�Ѵ�.
        // �׸��� Model�� Load, Clone���� �� �������� �̿��� ���۸� �����ϴ� ���̴�.
        VertexBuffer* m_pVertexBuffer;
        IndexBuffer* m_pIndexBuffer;

        BoundingBox m_BoundingBox;

        GameObject* m_pGameObject;
	};
}