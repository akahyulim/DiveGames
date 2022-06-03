#pragma once
#include "Mesh.h"
#include "Resource/Resource.h"

namespace Dive
{
	class GameObject;
	class Mesh;
	class VertexBuffer;
	class IndexBuffer;

	class Model : public Resource
	{
	public:
		RESOURCE_CLASS_TYPE(Model)
	public:
		Model();
		Model(unsigned long long id);
		//Model(const std::string& name = "", unsigned long long id = 0);
		~Model();

		void Clear();

		void UpdateGeometry();

		void AppendGeometry(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices,
			unsigned int* pOutVertexOffset = nullptr, unsigned int* pOutIndexOffset = nullptr);
		void GetGeometry(unsigned int vertexOffset, unsigned int vertexCount, std::vector<VertexType>* pOutVertices,
			unsigned int indexOffset, unsigned int indexCount, std::vector<unsigned int>* pOutIndices);
		const Mesh* GetMesh() const { return &m_Mesh; }

		VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

		void SetRootGameObject(GameObject* pGameObject) { m_pRootGameObject = pGameObject; }

		bool SaveToFile(const std::string& filepath) override;
		bool LoadFromFile(const std::string& filepath) override;

	private:
		bool createBuffers();

	private:
		GameObject* m_pRootGameObject = nullptr;
		
		Mesh m_Mesh;

		VertexBuffer* m_pVertexBuffer = nullptr;
		IndexBuffer* m_pIndexBuffer = nullptr;

		// 일단 저장만 한다. root의 transform scale에 곱하면 될 것 같은데
		// aabb를 이용해 직접 계산하는 부분이 있어 이해가 필요하다.
		float m_NormalizedScale = 1.0f;
	};
}