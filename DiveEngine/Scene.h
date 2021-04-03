#pragma once
#include "Object.h"
#include "Mesh.h"		// 추후 지워야할 대상?
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

		void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
		Mesh* GetMesh() { return m_pMesh; }


	private:
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		// camera??


		// test용
		Mesh* m_pMesh;
	};
}