#include "ResourceManager.h"
#include "../Helper/Log.h"

namespace dive
{
	ResourceManager::ResourceManager()
	{
		CORE_TRACE("ResourceManager::ResourceManager()");
	}

	ResourceManager::~ResourceManager()
	{
		for (auto pResource : m_Resources)
		{
			DV_DELETE(pResource);
		}
		m_Resources.clear();

		CORE_TRACE("ResourceManager::~ResourceManager()");

	}
	
	bool ResourceManager::IsCached(const std::string& name, eResourceType type)
	{
		return GetByName(name, type) != nullptr ? true : false;
	}
	
	Resource* ResourceManager::GetByName(const std::string& name, eResourceType type)
	{
		for (auto pResource : m_Resources)
		{
			if (pResource->GetName() == name && pResource->GetType() == type)
				return pResource;
		}

		return nullptr;
	}
}