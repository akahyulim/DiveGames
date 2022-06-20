#include "divepch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Base/Base.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene)
		: m_pScene(pScene)
	{
	}

	GameObject::~GameObject()
	{
		if (m_Components.empty())
			return;

		for(auto com : m_Components)
		{
			FIRE_EVENT(ModifyGameObjectEvent(static_cast<Component*>(com)));
			DV_DELETE(com);
		}
		m_Components.clear();
	}

	GameObject* GameObject::Clone()
	{
		if(!m_pScene)
			return nullptr;

		auto clone = m_pScene->CreateGameObject();
		clone->SetName(this->GetName());
		clone->SetActive(this->IsActive());

		if (HasComponent<Transform>())
		{
			auto cloneTransform = clone->GetComponent<Transform>();
			if (!cloneTransform)
				cloneTransform = clone->AddComponent<Transform>();

			this->GetComponent<Transform>()->CopyAttributes(cloneTransform);
		}
		if (HasComponent<MeshRenderable>())
		{
			auto cloneMeshRenderable = clone->AddComponent<MeshRenderable>();
			this->GetComponent<MeshRenderable>()->CopyAttributes(cloneMeshRenderable);
		}
		if (HasComponent<Light>())
		{
			auto cloneLight = clone->AddComponent<Light>();
			this->GetComponent<Light>()->CopyAttributes(cloneLight);
		}

		auto children = GetComponent<Transform>()->GetChildren();
		GameObject* childClone = nullptr;
		for (auto child : children)
		{
			childClone = child->GetGameObject()->Clone();
			childClone->GetComponent<Transform>()->SetParent(clone->GetComponent<Transform>());
		}

		return clone;
	}

	// 유니티 UI로 만들어 본 함수다. 이외에도 static 함수들이 몇 개 더 있다.
	// Instantiate()는 Object로 부터 상속된 static 함수다.
	GameObject* GameObject::CreatePrimitive(Scene* pScene, ePrimitiveType type)
	{
		if (pScene)
		{
			auto gameObj = pScene->CreateGameObject();
			gameObj->AddComponent<Transform>();
			gameObj->AddComponent<MeshRenderable>();

			// 결국 Meshfilter, Mesh가 존재한다. 즉, Resource가 있다는 거다.
			// 외부 파일이라면 Importer가 필요한데, 아마도 Engine format이거나
			// 코드화 된 mesh로 생성될 거다.

			return gameObj;
		}

		return nullptr;
	}
}