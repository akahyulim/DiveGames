#include "stdafx.h"
#include "Entity.h"

namespace Dive
{
	Entity::Entity(entt::entity handle, World* world)
		: m_EntityHandle(handle)
		, m_World(world)
	{
	}
}