#include "divepch.h"
#include "World.h"
#include "Core/Log.h"
#include "Core/Engine.h"

namespace Dive
{
	uint32_t World::s_WorldCounter = 0;

	World::World()
		: m_Name("World")
	{
	}

	World::~World()
	{
		DV_LOG(World, trace, "월드({:s})를 소멸자가 호출되었습니다.", m_Name);
	}

	// 존재하는 메서드들이 아니라고 한다...
	// 실제로는 동적생성하며 이를 Engine에서 배열로 관리한다.
	World* World::CreateWorld()
	{
		auto pNewWorld = new World;
		pNewWorld->Rename("World_" + std::to_string(s_WorldCounter++));

		return pNewWorld;
	}

	void World::DestroyWorld(World* pWorld)
	{
	}
}