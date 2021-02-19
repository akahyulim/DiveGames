#include "DivePch.h"
#include "ResourceManager.h"

namespace Dive
{
	ResourceManager::ResourceManager(Context * context)
		: Object(context)
	{
		// type�� directory ����

		// �̺�Ʈ ���?
	}

	ResourceManager::~ResourceManager()
	{
		// �̺�Ʈ ����?
	}
	
	std::shared_ptr<Resource> ResourceManager::Cache(const std::shared_ptr<Resource>& resource)
	{
		if (resource == nullptr || resource->GetResourceType() == eResourceType::UnKnown)
			return nullptr;

		// �̹� ����� ������ Ȯ��
		if (IsCached(resource->GetResourceType(), resource->GetResourceName()))
			return resource;

		// ����ȭ: ���� �̸��� ����� �Ѵ�.
		//resource->SaveToFile()

		return m_resources.emplace_back(resource);
	}

	bool ResourceManager::IsCached(const eResourceType type, const std::string & name)
	{
		if (name.empty())
		{
			CORE_ERROR("�߸��� �̸��� ���޹޾ҽ��ϴ�.");
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