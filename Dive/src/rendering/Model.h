#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Mesh;
	class GameObject;

	class Model : public Resource
	{
		DV_CLASS(Model, Resource)

	public:
		Model();
		~Model() override;

		bool LoadFromFile(const std::filesystem::path& path) override;
		bool SaveToFile(const std::filesystem::path& path) override;

		void Clear();

		GameObject* GetRootObject() const { return m_RootGameObject; }
		void SetRootObject(GameObject* rootGameObject) { m_RootGameObject = rootGameObject; }

		void AddMesh(Mesh* mesh) { m_Meshes.emplace_back(mesh); }
		std::vector<Mesh*>& GetAllMeshes() { return m_Meshes; }
		Mesh* GetMeshAt(UINT32 index = 0);
		Mesh* GetMeshByName(const std::string& name);

	private:
		GameObject* m_RootGameObject;

		// urho는 geometry뿐만 아니라 모든 Buffer도 관리한다.
		std::vector<Mesh*> m_Meshes;
	};
}