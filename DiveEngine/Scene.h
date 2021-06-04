#pragma once
#include "Object.h"
#include "Mesh.h"		// 추후 지워야할 대상
#include <vector>
#include <string>
#include <memory>

// 아무래도 Editor를 통해 직관적으로 구현 사항을 확인하는 편이 나을 것 같다.
namespace Dive
{
	class GameObject;

	// 추후 인터페이스를 개선하는 작업이 필요할 것 같다.
	// 너무 중구난방식으로 구현해놓았다.
	// 그런데 유니티에는 참고할만한 클래스가 없다...
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

		// test용
		void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
		Mesh* GetMesh() { return m_pMesh; }

	private:
		void gameObjectRemove(GameObject* pGameObject);


	private:
		// 이 경로는 Editor 혹은 App에서 설정해주는 것이 맞을 것 같다.
		// 그렇다면 Asset 폴더를 상위에서 설정해주는 것도 생각해 볼 수 있다.
		// 그러면 Scene에서는 해당 Asset에서 Scenes만 추가하거나 찾으면 되니깐 말이다.
		// 그런데 이건 어디까지나 Editor의 경우이다.
		// App의 경우엔 리소스를 어떻게 처리하지?
		std::string m_pathDir = "../Assets/Scenes/";
		std::string m_name = "Untitled";
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		// camera??


		// test용
		Mesh* m_pMesh = nullptr;
	};
}