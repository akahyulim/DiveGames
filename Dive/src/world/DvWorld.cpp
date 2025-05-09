#include "stdafx.h"
#include "DvWorld.h"
#include "systems/TransformSystem.h"

namespace Dive
{
	void DvWorld::Destroy()
	{
		m_EntityManager.Clear();
		
		for (auto system : m_Systems)
		{
			system->OnDestroy();
			DV_DELETE(system);
		}
		m_Systems.clear();
	}

	void DvWorld::Update()
	{
		for (auto system : m_Systems)
		{
			system->OnUpdate();
		}
	}
}