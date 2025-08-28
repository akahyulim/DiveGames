#include "stdafx.h"
#include "WorldManager.h"
#include "World.h"
#include "WorldSerializer.h"

namespace Dive
{
	World* WorldManager::s_activeWorld = nullptr;

	bool WorldManager::Initialize()
	{
		DV_SUBSCRIBE_EVENT(eEventType::Update, DV_EVENT_HANDLER_DATA_STATIC(OnUpdate));

		return true;
	}

	void WorldManager::Clear()
	{
		DV_LOG(WorldManager, info, "클리어 완료");
	}

	// 유니티의 경우 빈 이름, 이미 존재하는 Scene의 이름은 안된다.
	World* WorldManager::CreateWorld(const std::string& name)
	{
		s_activeWorld = new World(name);
		return s_activeWorld;
	}

	World* WorldManager::LoadWorld(const std::filesystem::path& filepath)
	{
		if (s_activeWorld)
			UnloadWorld();

		s_activeWorld = new World();
		WorldSerializer serializer(s_activeWorld);
		serializer.Deserialize(filepath);

		DV_FIRE_EVENT(eEventType::WorldLoaded);

		return s_activeWorld;
	}

	void WorldManager::UnloadWorld()
	{
		if (!s_activeWorld)
			return;

		// ResourceManager를 어떻게 할 것인가...

		DV_FIRE_EVENT(eEventType::WorldUnloaded);
	}

	void WorldManager::OnUpdate(EventData data)
	{
		if (!s_activeWorld)
			return;

		s_activeWorld->Update();
	}

	World* WorldManager::GetActiveWorld()
	{
		return s_activeWorld;
	}
}