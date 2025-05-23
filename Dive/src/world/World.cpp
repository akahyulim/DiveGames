#include "stdafx.h"
#include "World.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_Name(name)
	{
	}

	World::~World()
	{
		Clear();
	}

	// destroy와 clear에서 고민 중
	void World::Clear()
	{
		m_EntityManager.Clear();
		
		for (auto system : m_Systems)
		{
			system->OnDestroy();
			DV_DELETE(system);
		}
		m_Systems.clear();
	}

	void World::Update()
	{
		for (auto system : m_Systems)
		{
			system->OnUpdate();
		}
	}
}