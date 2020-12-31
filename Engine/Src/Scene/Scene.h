#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class GameObject;
	class E_UPDATE;

	// 추후 Scene Swap에 대해 생각해봐야 한다.
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object);

	public:
		explicit Scene(Context* context, std::string name = "");
		~Scene();

		void New() { clear(); }

		bool SaveToFile(const std::string& filepath);
		bool LoadFromFile(const std::string& filepath);

		// sub system interface를 만들것인가...
		void OnUpdate(const E_UPDATE* evnt);

		std::shared_ptr<GameObject> CreateGameObject(bool active = true);
		void RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);

		const std::shared_ptr<GameObject>& GetGameObjectByName(const std::string& name);
		const std::shared_ptr<GameObject>& GetGameObjectByID(unsigned int id);
		bool ExistsGameObject(const std::shared_ptr<GameObject>& gameObject);

		std::vector<std::shared_ptr<GameObject>> GetRootGameObjects();
		const std::vector<std::shared_ptr<GameObject>>& GetAllGameObjects() const { return m_gameObjects; }
		unsigned int GetGameObjectsCount()									const { return static_cast<unsigned int>(m_gameObjects.size()); }
 		
		std::string GetName()			const { return m_name; }
		void SetName(std::string name) { m_name = std::move(name); }
		bool IsDirty()					const { return m_bDirty; }

	private:
		Scene(const Scene&)				= delete;
		Scene& operator=(const Scene&)	= delete;

		void clear();
		void removeGameObject(const std::shared_ptr<GameObject> gameObject);

	private:
		std::string m_name;

		bool m_bDirty;	
		bool m_bClear;

		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		std::vector<std::shared_ptr<GameObject>> m_loadedGameObjects;
	};
}