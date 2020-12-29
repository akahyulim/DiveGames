#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class GameObject;
	class E_UPDATE;

	// �� ���� scene�� swap�� �� ������
	// �⺻������ file�� ���� �����ȴ�.
	// ���� unity�� scene swap�� �м��غ���.
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
		// resolve��� �ɷ� ��ä�Ǿ���. �Ź� ���ſ��θ� ����ϴ� ���� �ƴ϶�
		// �� �� ���� �� �ٷ�� �� ����.
		bool m_bDirty;	

		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	};
}