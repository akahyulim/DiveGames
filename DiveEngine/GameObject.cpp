#include "GameObject.h"
#include "Transform.h"

using namespace std;

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code())
	{
		// 하나로 만들까?
		auto transform = AddComponent<Transform>();
		transform->SetOwnder(this);
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_components)
		{
			component->Update(deltaTime);
		}
	}
}
