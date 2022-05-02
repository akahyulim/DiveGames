#include "divepch.h"
#include "ResourceManager.h"

namespace Dive
{
	void ResourceManager::Clear()
	{
		if (m_Resources.empty())
			return;

		for (auto pResource : m_Resources)
		{
			DV_DELETE(pResource);
		}
		m_Resources.clear();
		m_Resources.shrink_to_fit();
	}

	bool ResourceManager::HasResource(const std::string& name, const eResourceType type)
	{
		if(!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if ((pResource->GetName() == name) && (pResource->GetType() == type))
					return true;
			}
		}

		return false;
	}

	bool ResourceManager::HasResourceByID(unsigned long long id)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if (pResource->GetInstanceID() == id)
					return true;
			}
		}

		return false;
	}

	Resource* ResourceManager::GetResource(const std::string& name, const eResourceType type)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if ((pResource->GetName() == name) && (pResource->GetType() == type))
					return pResource;
			}
		}

		return nullptr;
	}

	Resource* ResourceManager::GetResourceByID(unsigned long long id)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if (pResource->GetInstanceID() == id)
					return pResource;
			}
		}

		return nullptr;
	}
}