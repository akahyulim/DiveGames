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
		UINT32 boneIDs[4]{};
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
            UINT32 vertexOffset,
            UINT32 vertexCount,
            UINT32 indexOffset,
            UINT32 indexCount,
            std::vector<UINT32>* indices,
            std::vector<VertexStatic>* vertices
        );

        void AddVertices(const std::vector<VertexStatic>& vertices, UINT32* outVertexOffset = nullptr);
        void AddIndices(const std::vector<UINT32>& indices, UINT32* outIndexOffset = nullptr);

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        std::vector<VertexStatic>& GetVertices() { return m_Vertices; }
        std::vector<UINT32>& GetIndices() { return m_Indices; }

        UINT32 GetVertexCount() const;
        UINT32 GetIndexCount() const;

        void CreateBuffers();
		std::shared_ptr<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
        std::shared_ptr<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
        
        void ComputeBouingBox();
        const BoundingBox& GetBoundingBox() const { return m_BoundingBox; }

        // mesh�κ��� gameObject�� renderer���� ������ �� �ִ�.
        GameObject* GetGameObject() const { return m_GameObject; }
        void SetGameObject(GameObject* pGameObject) { m_GameObject = pGameObject; }

    private:
        std::string m_Name;

        // 1. BonuingBox�� ������� ��� ���� ������ ������ �־�� �ϴ� ���� �´�.
        // 2. VertexSkinned�� �����Ϸ��� ��� Ŭ������ ����� ���� ���ƺ��δ�. 
        std::vector<VertexStatic> m_Vertices;
        std::vector<UINT32> m_Indices;

        // urho�� ������ �ڼ��� �������ڸ�
        // importer���� ����(ũ��, Ÿ��, ����Ÿ)�� �Ľ��� �� ���� ��ü�� ���常 �ϰ�
        // Model�� Save���� �� �������� ����ȭ�Ѵ�.
        // �׸��� Model�� Load, Clone���� �� �������� �̿��� ���۸� �����ϴ� ���̴�.
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

        BoundingBox m_BoundingBox;

        GameObject* m_GameObject;
	};
}