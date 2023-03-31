#pragma once
#include "Component.h"

namespace Dive
{
	class Model;

	class Drawable : public Component
	{
	public:
		Drawable(GameObject* pGameObject);
		~Drawable();

		void SetMesh(Model* pModel, const std::string& name, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount);

		Model* GetModel() const { return m_pModel; }

		std::string GetMeshName() const { return m_MeshName; }

		uint32_t GetMeshVertexOffset() const { return m_MeshVertexOffset; }
		uint32_t GetMeshVertexCount() const { return m_MeshVertexCount; }

		uint32_t GetMeshIndexOffset() const { return m_MeshIndexOffset; }
		uint32_t GetMeshIndexCount() const { return m_MeshIndexCount; }


	private:
		Model* m_pModel;

		std::string m_MeshName;

		uint32_t m_MeshVertexOffset;
		uint32_t m_MeshVertexCount;

		uint32_t m_MeshIndexOffset;
		uint32_t m_MeshIndexCount;
	};
}