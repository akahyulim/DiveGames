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
		DV_LOG(World, trace, "����({:s})�� �Ҹ��ڰ� ȣ��Ǿ����ϴ�.", m_Name);
	}

	// �����ϴ� �޼������ �ƴ϶�� �Ѵ�...
	// �����δ� ���������ϸ� �̸� Engine���� �迭�� �����Ѵ�.
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