#include "DivePch.h"
#include "ResourceManager.h"

namespace Dive
{
	ResourceManager::ResourceManager(Context * context)
		: Object(context)
	{
		// type별 directory 설정

		// 이벤트 등록?
	}

	ResourceManager::~ResourceManager()
	{
		// 이벤트 해제?
	}
	
	std::shared_ptr<Resource> ResourceManager::Cache(const std::shared_ptr<Resource>& resource)
	{
		if (resource == nullptr || resource->GetResourceType() == eResourceType::UnKnown)
			return nullptr;

		// 이미 저장된 것인지 확인
		if (IsCached(resource->GetResourceType(), resource->GetResourceName()))
			return resource;

		// 파일화: 파일 이름을 지녀야 한다.
		//resource->SaveToFile()

		return m_resources.emplace_back(resource);
	}

	bool ResourceManager::IsCached(const eResourceType type, const std::string & name)
	{
		if (name.empty())
		{
			CORE_ERROR("잘못된 이름을 전달받았습니다.");
			return false;
		}

		for (auto resource : m_resources)
		{
			if (GetByName(type, name))
				return true;
		}

		return false;
	}

	void ResourceManager::Remove(std::shared_ptr<Resource> resource)
	{
		if (!resource)
			return;

		if (!IsCached(resource->GetResourceType(), resource->GetResourceName()))
			return;

		for (auto it = m_resources.begin(); it != m_resources.end();)
		{
			if (resource->GetID() == (*it)->GetID())
			{
				it = m_resources.erase(it);
				return;
			}
			else
				it++;
		}
	}

	std::shared_ptr<Resource> ResourceManager::GetByName(const eResourceType type, const std::string & name)
	{
		for (auto resource : m_resources)
		{
			if (resource->GetResourceType() == type && resource->GetResourceName() == name)
			{
				return resource;
			}
		}

		return std::shared_ptr<Resource>();
	}

	std::vector<std::shared_ptr<Resource>> ResourceManager::GetByType(eResourceType type)
	{
		std::vector<std::shared_ptr<Resource>> resources;

		for (auto resource : m_resources)
		{
			if (resource->GetResourceType() == type)
			{
				resources.emplace_back(resource);
			}
		}
		
		return resources;
	}

	unsigned int ResourceManager::GetResourceCount(eResourceType type)
	{
		return static_cast<unsigned int>(GetByType(type).size());
	}
}