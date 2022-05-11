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

	// ����Ƽ UI�� ����� �� �Լ���. �̿ܿ��� static �Լ����� �� �� �� �ִ�.
	// Instantiate()�� Object�� ���� ��ӵ� static �Լ���.
	GameObject* GameObject::CreatePrimitive(Scene* pScene, ePrimitiveType type)
	{
		if (pScene)
		{
			auto pGameObject = pScene->CreateGameObject();
			pGameObject->AddComponent<Transform>();
			pGameObject->AddComponent<MeshRenderable>();

			// �ᱹ Meshfilter, Mesh�� �����Ѵ�. ��, Resource�� �ִٴ� �Ŵ�.
			// �ܺ� �����̶�� Importer�� �ʿ��ѵ�, �Ƹ��� Engine format�̰ų�
			// �ڵ�ȭ �� mesh�� ������ �Ŵ�.

			return pGameObject;
		}

		return nullptr;
	}
}