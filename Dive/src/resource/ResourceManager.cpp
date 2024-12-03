#include "stdafx.h"
#include "ResourceManager.h"

namespace Dive
{
	std::mutex ResourceManager::s_Mutex;
	std::unordered_map<uint32_t, std::vector<std::shared_ptr<Resource>>> s_Resources;

	void ResourceManager::Shutdown()
	{
		s_Resources.clear();
	}

	uint32_t ResourceManager::GetAllResourceCount()
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		uint32_t count = 0;
		for (const auto& [typeId, resources] : s_Resources)
		{
			count += static_cast<uint32_t>(resources.size());
		}

		return count;
	}

	bool ResourceManager::IsCached(uint64_t id)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		for (const auto& [typeId, resources] : s_Resources)
		{
			for (const auto& pResource : resources)
			{
				if (pResource->GetId() == id)
					return true;
			}
		}

		return false;
	}
}