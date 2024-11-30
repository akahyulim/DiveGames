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

        // 현재 사용하는 곳이 없다.
        // 메시에서 특정 부분의 정점, 인덱스를 리턴
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

        // mesh로부터 gameObject와 renderer까지 접근할 수 있다.
        GameObject* GetGameObject() const { return m_pGameObject; }
        void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }

    private:
        std::string m_Name;

        // 1. BonuingBox를 만들려면 적어도 정점 정보는 가지고 있어야 하는 것이 맞다.
        // 2. VertexSkinned를 관리하려면 상속 클래스를 만드는 편이 나아보인다. 
        std::vector<VertexStatic> m_Vertices;
        std::vector<uint32_t> m_Indices;

        // urho의 구현을 자세히 설명하자면
        // importer에서 정보(크기, 타입, 데이타)를 파싱한 후 버퍼 객체에 저장만 하고
        // Model의 Save에서 이 정보들을 파일화한다.
        // 그리고 Model의 Load, Clone에서 이 정보들을 이용해 버퍼를 생성하는 것이다.
        VertexBuffer* m_pVertexBuffer;
        IndexBuffer* m_pIndexBuffer;

        BoundingBox m_BoundingBox;

        GameObject* m_pGameObject;
	};
}