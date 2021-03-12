#pragma once
#include "Object.h"
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

		GameObject* CreateGameObject();
		GameObject* GetGameObjectByName(const std::string& name);
		void RemoveGameObjectByName(const std::string& name);
		void RemoveGameObject(GameObject* target);

		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		
	private:
		Scene(const Scene&)				= delete;
		Scene& operator=(const Scene&)	= delete;

	private:
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	};
}