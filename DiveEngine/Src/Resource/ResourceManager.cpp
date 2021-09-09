#include "ResourceManager.h"
#include "../Log.h"

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
	
	// 타입까지 확인해야 하는게 맞지만
	// 현재 비교가 되지 않고 있다.
	Resource* ResourceManager::GetByName(const std::string& name, eResourceType type)
	{
		for (auto pResource : m_Resources)
		{
			//if (pResource->GetName() == name && pResource->GetType() == type)//static_cast<eResourceType>(type))
			if(pResource->GetName() == name)
				return pResource;
		}

		return nullptr;
	}
}