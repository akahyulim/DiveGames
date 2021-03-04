#pragma once
#include "Object.h"
#include <vector>
#include <string>

namespace Dive
{
	class GameObject;

	class Scene : public Object
	{
	public:
		Scene() = default;
		~Scene();

		void Update(float deltaTime);
		void Clear();

		GameObject* CreateGameObject();
		GameObject* GetGameObject(const std::string& name);
		void RemoveGameObject(const std::string& name);

		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

	private:
		Scene(const Scene&)				= delete;
		Scene& operator=(const Scene&)	= delete;

	private:
		std::vector<GameObject*> m_gameObjects;
	};
}