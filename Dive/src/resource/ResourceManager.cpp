#include "stdafx.h"
#include "ResourceManager.h"

namespace Dive
{
	ResourceManager* GResourceManager = ResourceManager::GetInstance();

	ResourceManager* ResourceManager::s_pInstance = nullptr;
	std::once_flag ResourceManager::s_OnceFlag;

	void ResourceManager::Shutdown()
	{
		m_Resources.clear();
	}

	uint32_t ResourceManager::GetAllResourceCount()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		uint32_t count = 0;
		for (const auto& [typeId, resources] : m_Resources)
			count += static_cast<uint32_t>(resources.size());

		return count;
	}

	bool ResourceManager::IsCached(uint64_t id)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		for (const auto& [typeId, resources] : m_Resources)
		{
			for (const auto& resource : resources)
			{
				if (resource->GetId() == id)
					return true;
			}
		}

		return false;
	}
}