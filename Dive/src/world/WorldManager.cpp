#include "stdafx.h"
#include "WorldManager.h"
#include "World.h"
#include "WorldSerializer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	std::unique_ptr<World> WorldManager::s_ActiveWorld;

	World* WorldManager::CreateWorld(const std::string& name)
	{
		if (s_ActiveWorld && s_ActiveWorld->GetName() == name)
		{
			DV_LOG(WorldManager, warn, "�̹� �����ϴ� ����� ������ �̸��� ������ ������ �� �����ϴ� : {:s}", name);
			return nullptr;
		}

		s_ActiveWorld = std::make_unique<World>(name);
		return s_ActiveWorld.get();
	}

	World* WorldManager::LoadFromFile(const std::filesystem::path& path)
	{
		s_ActiveWorld = std::make_unique<World>();
		WorldSerializer serializer(s_ActiveWorld.get());
		if (!serializer.Deserialize(path))
		{
			s_ActiveWorld.reset();
			return nullptr;
		}

		return s_ActiveWorld.get();
	}

	void WorldManager::SaveToFile(const std::filesystem::path& path)
	{
		if (path.empty() && s_ActiveWorld->GetFilePath().empty())
		{
			DV_LOG(WorldManager, warn, "������ ���� ��θ� �����ؾ� �մϴ�.");
			return;
		}

		WorldSerializer serializer(s_ActiveWorld.get());
		serializer.Serialize(path.empty() ? s_ActiveWorld->GetFilePath() : path);
	}

	void WorldManager::UnloadWorld()
	{
		s_ActiveWorld.reset();
	}
}