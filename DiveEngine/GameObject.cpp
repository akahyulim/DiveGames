#include "GameObject.h"

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code()),
		m_pTransform(nullptr)
	{
		// 하나로 만들까?
		m_pTransform = AddComponent<Transform>();
		m_pTransform->SetOwnder(this);
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_components)
		{
			component->Update(deltaTime);
		}
	}
}
