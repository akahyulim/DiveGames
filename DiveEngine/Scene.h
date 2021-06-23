#pragma once
#include "Object.h"
#include "Mesh.h"		// 추후 지워야할 대상
#include <vector>
#include <string>
#include <memory>

namespace dive
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

		std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() { return mGameObjects; }
		std::vector<GameObject*> GetRootGameObjects();

		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(mGameObjects.size()); }

		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		std::string GetName() { return mName; }
		void SetName(const std::string& name) { mName = name; }

		bool IsDirty() { return mbDirty; }

		// test용
		void SetMesh(Mesh* mesh) { mMesh = mesh; }
		Mesh* GetMesh() { return mMesh; }

	private:
		void gameObjectRemove(GameObject* gameObject);


	private:
		std::string mName;
		std::vector<std::shared_ptr<GameObject>> mGameObjects;
		// camera??

		bool mbDirty;


		// test용
		Mesh* mMesh = nullptr;
	};
}