#pragma once
#include "Component.h"

namespace Dive
{
	class Model;
	class Material;

	// 현재는 Material 및 특정 메시에서 그려져야 하는 부분의 offset을 관리하는데 사용된다.
	// 목표는 Mesh, Bone에 Animate?, Draw를 구현해 놓고
	// Drawable 혹은 Static or SkinnedMeshRender로 이름을 바꾼 후 이들을 관리토록 하고 싶다.
	// 아니면 변경할 이름에 걸맞게 Mesh, Bone에서 데이터를 가져와 직접 Draw하도록 만들어도 될 듯 하다.

	// 현재는 MeshNode의 Parent가 Root라 localPosition으로 그려진다.
	// 하지만 추후엔 Bone의 OffsetMatrix 누적변환이 각 정점에 weight만큼 변환될 것이다.
	class Drawable : public Component
	{
	public:
		Drawable(GameObject* pGameObject);
		~Drawable();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void SetMesh(Model* pModel, const std::string& name, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount);

		Model* GetModel() const { return m_pModel; }

		std::string GetMeshName() const { return m_MeshName; }

		uint32_t GetMeshVertexOffset() const { return m_MeshVertexOffset; }
		uint32_t GetMeshVertexCount() const { return m_MeshVertexCount; }

		uint32_t GetMeshIndexOffset() const { return m_MeshIndexOffset; }
		uint32_t GetMeshIndexCount() const { return m_MeshIndexCount; }

		bool HasMaterial() const { return m_pMaterial; }
		Material* GetMaterial() const { return m_pMaterial; }
		void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }
		void SetMaterial(const std::string& filePath);

	private:
		Model* m_pModel;

		std::string m_MeshName;

		uint32_t m_MeshVertexOffset;
		uint32_t m_MeshVertexCount;

		uint32_t m_MeshIndexOffset;
		uint32_t m_MeshIndexCount;

		Material* m_pMaterial;
	};
}