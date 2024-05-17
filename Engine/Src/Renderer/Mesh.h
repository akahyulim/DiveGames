#pragma once
#include "Math/BoundingBox.h"

namespace Dive
{
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

        // urho�� �� �κ��� ���� ���ο� ����.
        // ���۸� �ܺο��� ���� ������ �� mesh�� ���Խ�Ű�� ���´�.
        // �׷��� geometry�� �� raw data��� ������ �ִ�... ������ vertex element����...
        // => ���� urho�� ������ �ʿ�� ���ٴ� ����� ���ȴ�.
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