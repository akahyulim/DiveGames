#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class GameObject;
	class E_UPDATE;

	// 두 개의 scene이 swap될 수 있으나
	// 기본적으로 file을 통해 관리된다.
	// 추후 unity의 scene swap을 분석해보자.
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object);

	public:
		Scene(Context* context, std::string name = "");
		~Scene();

		bool SaveToFile(const std::string& filepath);
		bool LoadFromFile(const std::string& filepath);

		void OnUpdate(const E_UPDATE* evnt);

		void Unload();

		std::shared_ptr<GameObject> CreateGameObject();
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
		Scene(const Scene&)				= delete;
		Scene& operator=(const Scene&)	= delete;

		void removeGameObject(const std::shared_ptr<GameObject> gameObject);

	private:
		std::string m_name;
		// resolve라는 걸로 대채되었다. 매번 갱신여부를 기록하는 것이 아니라
		// 한 번 묶은 후 다루는 것 같다.
		bool m_bDirty;	

		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	};
}