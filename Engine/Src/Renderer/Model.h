#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Mesh;
	class GameObject;

	class Model : public Resource
	{
		DV_CLASS(Model, Resource);

	public:
		Model();
		~Model() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool SaveToFile(const std::string& fileName) override;

		void Clear();

		GameObject* GetRootObject() const { return m_pRootObject; }
		void SetRootObject(GameObject* pRootObject) { m_pRootObject = pRootObject; }

		void AddMesh(Mesh* pMesh) { m_Meshes.emplace_back(pMesh); }
		std::vector<Mesh*>& GetAllMeshes() { return m_Meshes; }
		Mesh* GetMeshAt(uint32_t index = 0);
		Mesh* GetMeshByName(const std::string& name);

	private:
		GameObject* m_pRootObject;

		std::vector<Mesh*> m_Meshes;
	};
}