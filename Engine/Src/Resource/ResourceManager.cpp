#include "divepch.h"
#include "ResourceManager.h"
#include "../Helper/Log.h"

namespace Dive
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

	// Asset 폴더의 모든 Resource를 로드한다.
	void ResourceManager::LoadResourcesFromAssetFolder()
	{
	}
	
	// 현재 Scene에서 사용한 Resource들의 파일 경로를 저장한다.
	// WorldSave Event에서 호출된다.
	void ResourceManager::SaveResourcesToFile(const std::string& filepath)
	{
	}
	
	// Scene에 사용되는 Resource들의 파일 경로를 읽어 로드한다.
	// WorldLoad Event에서 호출되는데... 애매하다.
	void ResourceManager::LoadResourcesFromFile(const std::string& filepath)
	{
	}
}