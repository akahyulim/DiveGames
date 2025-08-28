#include "stdafx.h"
#include "ResourceManager.h"
#include "graphics/Texture2D.h"

namespace Dive
{
	std::unordered_map<eResourceType, std::vector<std::shared_ptr<Resource>>> ResourceManager::s_resources;

	void ResourceManager::Clear()
	{
		s_resources.clear();
	}

	bool ResourceManager::IsCached(std::shared_ptr<Resource> resource)
	{
		assert(resource);
		return IsCached(resource->GetInstanceID());
	}

	bool ResourceManager::IsCached(uint64_t instanceID)
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
