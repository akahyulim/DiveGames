#include "stdafx.h"
#include "ResourceManager.h"
#include "graphics/Texture2D.h"

namespace Dive
{
	std::unordered_map<eResourceType, std::vector<std::unique_ptr<Resource>>> ResourceManager::s_resources;

	bool ResourceManager::Initialize()
	{
		//SetResourceFolder("Assets");

		// texture2D
		Load<Texture2D>("Assets/Textures/dmc.jpg");
		Load<Texture2D>("Assets/Textures/DokeV.jpeg");
		Load<Texture2D>("Assets/Textures/relaxed_morning.jpg");
		Load<Texture2D>("Assets/Textures/sky_daytime_blue.jpg");
		Load<Texture2D>("Assets/Textures/stone01.tga");

		return true;
	}

	void ResourceManager::Clear()
	{
		s_resources.clear();

		DV_LOG(ResourceManager, info, "클리어 완료");
	}

	bool ResourceManager::IsCahed(UINT64 instanceID)
	{
		for (const auto& [type, resources] : s_resources)
		{
			for (auto& resource : resources)
			{
				if (resource->GetInstanceID() == instanceID)
					return true;
			}
		}
		return false;
	}

	uint32_t ResourceManager::GetAllResourceCount()
	{
		uint32_t count = 0;
		for (const auto& [type, resources] : s_resources)
			count += static_cast<uint32_t>(resources.size());
		return count;
	}
}
