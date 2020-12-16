#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class GameObject;

	// subsystem이어야 하는가...
	// 둘 이상의 Scene이 필요할 수 있는데...
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object);

	public:
		Scene(Context* context, std::string name = "");
		~Scene();

		void Unload();

		std::shared_ptr<GameObject>& CreateGameObject();
		void AddGameObject(const std::shared_ptr<GameObject>& gameObject);
		void RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);

		const std::shared_ptr<GameObject>& GetGameObjectByName(const std::string& name);
		const std::shared_ptr<GameObject>& GetGameObjectByID(unsigned int id);
		bool ExistsGameObject(const std::shared_ptr<GameObject>& gameObject);

		std::vector<std::shared_ptr<GameObject>> GetRootGameObjects();
		const std::vector<std::shared_ptr<GameObject>>& GetAllGameObjects() const { return m_gameObjects; }
		size_t GetGameObjectsCount()										const { return m_gameObjects.size(); }
 		
		std::string GetName()			const { return m_name; }
		void SetName(std::string name) { m_name = std::move(name); }
		bool IsDirty()					const { return m_bDirty; }

	private:
	private:
		std::string m_name;
		bool m_bDirty;

		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	};
}