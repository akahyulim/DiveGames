#pragma once
#include "Core/CoreDefs.h"
#include "Graphics/GraphicsDefs.h"
#include "Resource/Resource.h"


namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class GameObject;
	class IMesh;
	
	// 디버그 렌더링을 위해 class로 만들어 Draw를 추가할 수 있다.
	struct BoneInfo
	{
		int32_t id;
		DirectX::XMFLOAT4X4 offsetMatrix;
	};

	class Model : public Resource
	{
	public:
		Model();
		~Model();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void Clear();

		// mesh로 변경 ===============================================================================
		void AddVertices(std::vector<VertexStatic>& vertices, uint32_t* pOffset = nullptr);
		void AddIndices(std::vector<uint32_t>& indices, uint32_t* pOffset = nullptr);

		std::vector<VertexStatic>& GetVertices() { return m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return m_Indices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		template<class T>
		void AppendMesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, uint32_t* pVertexOffset = nullptr, uint32_t* pIndexOffset = nullptr) const;

		//============================================================================================

		std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }

		bool CreateBuffers();
		VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pRoot) { m_pRootGameObject = pRoot; }

	private:
	private:
		// GameObject를 상속한 다른 객체였으면 좋겠다.
		GameObject* m_pRootGameObject;

		// 적어도 static과 skinned 두 타입으로 구분해야 한다.
		std::vector<VertexStatic> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<IMesh*> m_Meshes;		// urho는 lod까지 적용해서 vector가 둘이다.

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		// 특정 상황에서 버퍼가 둘 이상으로 나누어질 수 있다.
		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};
	
	template<class T>
	void Model::AppendMesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, uint32_t* pVertexOffset, uint32_t* pIndexOffset) const
	{
		DV_ASSERT(!vertices.empty());
		DV_ASSERT(!indices.empty());

	}
}