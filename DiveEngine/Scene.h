#pragma once
#include "Object.h"
#include "Mesh.h"		// 추후 지워야할 대상
#include <vector>
#include <string>
#include <memory>

namespace Dive
{
	class GameObject;

	class Scene : public Object
	{
	public:
		Scene();
		~Scene();

		void Update(float deltaTime);

		void Clear();

		bool SaveToFile(const std::string& filepath);
		bool LoadFromFile(const std::string& filepath);

		GameObject* CreateGameObject();
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByID(unsigned int id);
		void RemoveGameObject(GameObject* target);

		std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return m_gameObjects; }
		std::vector<GameObject*> GetRootGameObjects();

		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(m_gameObjects.size()); }

		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		std::string GetName() { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

		bool IsDirty() { return m_bDirty; }

		// test용
		void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
		Mesh* GetMesh() { return m_pMesh; }

	private:
		void gameObjectRemove(GameObject* pGameObject);


	private:
		std::string m_name;
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		// camera??

		bool m_bDirty;


		// test용
		Mesh* m_pMesh = nullptr;
	};
}