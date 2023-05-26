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

	class Model : public Resource
	{
	public:
		Model();
		~Model();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void Clear();

		// mesh�� ���� ===============================================================================
		void AddVertices(std::vector<VertexStatic>& vertices, uint32_t* pOffset = nullptr);
		void AddIndices(std::vector<uint32_t>& indices, uint32_t* pOffset = nullptr);

		std::vector<VertexStatic>& GetVertices() { return m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return m_Indices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		template<class T>
		void AppendMesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, uint32_t* pVertexOffset = nullptr, uint32_t* pIndexOffset = nullptr) const;

		//============================================================================================

		bool CreateBuffers();
		VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pRoot) { m_pRootGameObject = pRoot; }

	private:
	private:
		// GameObject�� ����� �ٸ� ��ü������ ���ڴ�.
		GameObject* m_pRootGameObject;

		// ��� static�� skinned �� Ÿ������ �����ؾ� �Ѵ�.
		std::vector<VertexStatic> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<IMesh*> m_Meshes;		// urho�� lod���� �����ؼ� vector�� ���̴�.

		// Ư�� ��Ȳ���� ���۰� �� �̻����� �������� �� �ִ�.
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