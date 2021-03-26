#include "GameObject.h"
#include "Transform.h"

using namespace std;

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code())
	{
		AddComponent<Transform>();
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_components)
		{
			component->Update(deltaTime);
		}
	}
}
