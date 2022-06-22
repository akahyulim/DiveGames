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
		if (!pScene)
			return nullptr;

		auto pGameObject = pScene->CreateGameObject();
		auto pMeshRenderable = pGameObject->AddComponent<MeshRenderable>();

		// 최초엔 Model을 직접 생성해 버퍼를 구성하는 것이 맞다.
		// 하지만 이후엔 복사를 통해 기존 버퍼를 재사용해야 한다.
		// 이는 커스텀 오브젝트 역시 마찬가지다.
		// 오리지날을 따로 관리하지 않는다면 삭제했을 경우 복제본들에 문제가 생긴다.
		switch (type)
		{
		case ePrimitiveType::Sphere:
			pGameObject->SetName("Sphere");
			break;

		case ePrimitiveType::Capsule:
			pGameObject->SetName("Capsule");
			break;

		case ePrimitiveType::Cylinder:
			pGameObject->SetName("Cylinder");
			break;

		case ePrimitiveType::Cube:
			pGameObject->SetName("Cube");
			break;

		case ePrimitiveType::Plane:
			pGameObject->SetName("Plane");
			break;

		case ePrimitiveType::Quad:
			pGameObject->SetName("Quad");
			break;
		}

		pMeshRenderable->UseDefaultMaterial();

		return pGameObject;
	}
}