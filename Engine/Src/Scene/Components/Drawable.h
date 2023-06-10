#pragma once
#include "Component.h"

namespace Dive
{
	class Model;
	class Material;

	// ����� Material �� Ư�� �޽ÿ��� �׷����� �ϴ� �κ��� offset�� �����ϴµ� ���ȴ�.
	// ��ǥ�� Mesh, Bone�� Animate?, Draw�� ������ ����
	// Drawable Ȥ�� Static or SkinnedMeshRender�� �̸��� �ٲ� �� �̵��� ������� �ϰ� �ʹ�.
	// �ƴϸ� ������ �̸��� �ɸ°� Mesh, Bone���� �����͸� ������ ���� Draw�ϵ��� ���� �� �� �ϴ�.

	// ����� MeshNode�� Parent�� Root�� localPosition���� �׷�����.
	// ������ ���Ŀ� Bone�� OffsetMatrix ������ȯ�� �� ������ weight��ŭ ��ȯ�� ���̴�.
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