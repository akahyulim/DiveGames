#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;
	class Component;

	class Scene
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void New();
		static void Clear();

		static void Update(float delta);

		static bool LoadFromFile(const std::string& filePath);
		static bool SaveToFile(const std::string& filePath);

		static std::string GetName();
		static std::string GetFilepath();

		static GameObject* CreateGameObject(const std::string& name = "GameObject");
		static void RemoveGameObject(GameObject* pGameObject);
		static void RemoveGameObjectByID(uint64_t id);
		static GameObject* GetGameObjectByID(uint64_t id);
		static bool ExistsGameObject(GameObject* pGameObject);
		static bool ExistsGameObjectByID(uint64_t id);
		static std::vector<GameObject*> GetRootGameObjects();
		static std::vector<GameObject*> GetAllGameObjects();
		static uint64_t GetGameObjectsCount();

		static void RegisterComponent(Component* pComponent, uint64_t id = 0);
		static void DeregisterComponent(Component* pComponent);
		static void DeregisterComponentByID(uint64_t id);
		static Component* GetComponent(uint64_t id);

	private:
		static uint64_t getFreeGameObjectID();
		static uint64_t getFreeComponentID();
	};
}