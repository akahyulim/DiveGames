#include "divepch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Base/Base.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene, const std::string& name, unsigned long long id)
		: m_pScene(pScene), Object(name, id)
	{
	}

	GameObject::~GameObject()
	{
		if (m_Components.empty())
			return;

		for(auto pComponent : m_Components)
		{
			FIRE_EVENT(ModifyGameObjectEvent(static_cast<Component*>(pComponent)));
			DV_DELETE(pComponent);
		}
		m_Components.clear();
	}

	// 유니티 UI로 만들어 본 함수다. 이외에도 static 함수들이 몇 개 더 있다.
	// Instantiate()는 Object로 부터 상속된 static 함수다.
	GameObject* GameObject::CreatePrimitive(Scene* pScene, ePrimitiveType type)
	{
		if (pScene)
		{
			auto pGameObject = pScene->CreateGameObject();
			pGameObject->AddComponent<Transform>();
			pGameObject->AddComponent<MeshRenderable>();

			// 결국 Meshfilter, Mesh가 존재한다. 즉, Resource가 있다는 거다.
			// 외부 파일이라면 Importer가 필요한데, 아마도 Engine format이거나
			// 코드화 된 mesh로 생성될 거다.

			return pGameObject;
		}

		return nullptr;
	}
}