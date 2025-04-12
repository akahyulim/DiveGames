#include "stdafx.h"
#include "GameObject.h"

namespace Dive
{
	GameObject::GameObject(entt::entity handle, World* world)
		: m_EntityHandle(handle)
		, m_World(world)
	{
	}
}