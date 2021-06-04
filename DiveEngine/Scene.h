#pragma once
#include "Object.h"
#include "Mesh.h"		// ���� �������� ���
#include <vector>
#include <string>
#include <memory>

// �ƹ����� Editor�� ���� ���������� ���� ������ Ȯ���ϴ� ���� ���� �� ����.
namespace Dive
{
	class GameObject;

	// ���� �������̽��� �����ϴ� �۾��� �ʿ��� �� ����.
	// �ʹ� �߱���������� �����س��Ҵ�.
	// �׷��� ����Ƽ���� �����Ҹ��� Ŭ������ ����...
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

		// test��
		void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
		Mesh* GetMesh() { return m_pMesh; }

	private:
		void gameObjectRemove(GameObject* pGameObject);


	private:
		// �� ��δ� Editor Ȥ�� App���� �������ִ� ���� ���� �� ����.
		// �׷��ٸ� Asset ������ �������� �������ִ� �͵� ������ �� �� �ִ�.
		// �׷��� Scene������ �ش� Asset���� Scenes�� �߰��ϰų� ã���� �Ǵϱ� ���̴�.
		// �׷��� �̰� �������� Editor�� ����̴�.
		// App�� ��쿣 ���ҽ��� ��� ó������?
		std::string m_pathDir = "../Assets/Scenes/";
		std::string m_name = "Untitled";
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		// camera??


		// test��
		Mesh* m_pMesh = nullptr;
	};
}