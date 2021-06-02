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

	class Scene : public Object
	{
	public:
		Scene();
		~Scene();

		void Update(float deltaTime);

		void Clear();

		// ��ΰ� �ʿ��ϴ�. �Ƹ��� Asset�� �°���.
		// ���� helper�� �ʿ��ϴ�.
		void LoadFromFile(const std::string& filepath);
		void SaveToFile(const std::string& filepath);

		GameObject* CreateGameObject();
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByID(unsigned int id);
		void RemoveGameObjectByName(const std::string& name);
		void RemoveGameObject(GameObject* target);

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