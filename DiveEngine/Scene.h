#pragma once
#include "Object.h"
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

		GameObject* CreateGameObject(bool active = true);
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByID(unsigned int id);
		void RemoveGameObject(GameObject* gameObject);

		const std::vector<GameObject*>& GetAllGameObjects() const { return mGameObjects; }
		std::vector<GameObject*> GetRootGameObjects() const;

		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(mGameObjects.size()); }

		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		bool IsDirty() { return mbDirty; }

	private:
		void eraseGameObject(GameObject* gameObject);

	private:
		std::vector<GameObject*> mGameObjects;
		// camera??

		bool mbDirty;
	};
}