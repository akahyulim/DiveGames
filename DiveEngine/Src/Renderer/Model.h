#pragma once
#include "RendererDefs.h"
#include "Resource/Resource.h"


namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class GameObject;

	class Model : public Resource
	{
	public:
		Model();
		~Model();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void Clear();

		void AddVertices(std::vector<VertexModel>& vertices, uint32_t* pOffset = nullptr);
		void AddIndices(std::vector<uint32_t>& indices, uint32_t* pOffset = nullptr);

		std::vector<VertexModel>& GetVertices() { return m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return m_Indices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		bool CreateBuffers();
		VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pRoot) { m_pRootGameObject = pRoot; }

	private:
	private:
		GameObject* m_pRootGameObject;

		std::vector<VertexModel> m_Vertices;
		std::vector<uint32_t> m_Indices;

		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};
}