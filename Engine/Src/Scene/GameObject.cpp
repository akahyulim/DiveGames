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

	// ����Ƽ UI�� ����� �� �Լ���. �̿ܿ��� static �Լ����� �� �� �� �ִ�.
	// Instantiate()�� Object�� ���� ��ӵ� static �Լ���.
	GameObject* GameObject::CreatePrimitive(Scene* pScene, ePrimitiveType type)
	{
		if (pScene)
		{
			auto gameObj = pScene->CreateGameObject();
			gameObj->AddComponent<Transform>();
			gameObj->AddComponent<MeshRenderable>();

			// �ᱹ Meshfilter, Mesh�� �����Ѵ�. ��, Resource�� �ִٴ� �Ŵ�.
			// �ܺ� �����̶�� Importer�� �ʿ��ѵ�, �Ƹ��� Engine format�̰ų�
			// �ڵ�ȭ �� mesh�� ������ �Ŵ�.

			return gameObj;
		}

		return nullptr;
	}
}