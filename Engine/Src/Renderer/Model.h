#pragma once
#include "Mesh.h"

namespace Dive
{
	class GameObject;
	class Mesh;

	// 현재 이름을 설정하지 않고 있다.
	class Model
	{
	public:
		Model();
		~Model();

		void Clear();

		void UpdateGeometry();

		void AppendGeometry(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices,
			unsigned int* pOutVertexOffset, unsigned int* pOutIndexOffset);
		void GetGeometry(unsigned int vertexOffset, unsigned int vertexCount, std::vector<VertexType>* pOutVertices,
			unsigned int indexOffset, unsigned int indexCount, std::vector<unsigned int>* pOutIndices);
		const Mesh* GetMesh() const { return &m_Mesh; }

		const ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer; }
		const ID3D11Buffer* GetIndexBuffer() const { return m_pIndexBuffer; }

		void SetRootGameObject(GameObject* pGameObject) { m_pRootGameObject = pGameObject; }

		std::string GetName() const { return m_Name; }

	private:
		bool createBuffers();

	private:
		std::string m_Name = "Unknown";

		GameObject* m_pRootGameObject = nullptr;
		
		Mesh m_Mesh;

		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIndexBuffer = nullptr;
	};
}