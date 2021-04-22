#include "GameObject.h"
#include "Transform.h"

using namespace std;

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code())
	{
		// �ϳ��� �����?
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
