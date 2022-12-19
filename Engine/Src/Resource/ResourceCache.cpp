#include "divepch.h"
#include "ResourceCache.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive 
{
	ResourceCache::ResourceCache(Context* pContext)
		: Object(pContext)
	{
	}

	ResourceCache::~ResourceCache()
	{
		RemoveAllResources();


		DV_LOG_ENGINE_TRACE("ResourceCache 소멸 완료");
	}

	// 매뉴얼리소스는 반드시 이름이 존재해야 한다.
	bool ResourceCache::AddManualResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_LOG_ENGINE_ERROR("ResourceCache::AddManualResource - 잘못된 인자를 전달받아 리소스를 추가할 수 없습니다.");
			return false;
		}

		if (pResource->GetName().empty())
		{
			DV_LOG_ENGINE_ERROR("ResourceCache::AddManualResource - 전달받은 리소스에 이름이 존재하지 않아 추가할 수 없습니다.");
			return false;
		}

		m_ResourceGroups[pResource->GetTypeHash().GetValue()][pResource->GetNameHash().GetValue()] = pResource;

		return true;
	}

	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		auto fileName = FileSystem::GetFileNameAndExtension(name);
		StringHash nameHash(fileName);

		auto* pExistingResource = findResource(type, nameHash);
		if (pExistingResource)
			return pExistingResource;

		auto* pNewResource = dynamic_cast<Resource*>(m_pContext->CreateObject(type));
		if (!pNewResource->LoadFromFile(name))
		{
			DV_DELETE(pNewResource);
			return nullptr;
		}

		pNewResource->SetName(fileName);
		
		m_ResourceGroups[type.GetValue()][nameHash.GetValue()] = pNewResource;

		return pNewResource;
	}

	void ResourceCache::GetResources(StringHash type, std::vector<Resource*>& outResources)
	{
		outResources.clear();

		auto i = m_ResourceGroups.find(type.GetValue());
		if (i != m_ResourceGroups.end())
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				outResources.emplace_back(j->second);
			}
		}
	}

	Resource* ResourceCache::GetExistingResource(StringHash type, const std::string& name)
	{
		auto fileName = FileSystem::GetFileNameAndExtension(name);
		auto nameHash = StringHash(fileName);

		return findResource(type, nameHash);
	}

	bool ResourceCache::IsExistingResource(StringHash type, const std::string& name)
	{
		auto fileName = FileSystem::GetFileNameAndExtension(name);
		auto nameHash = StringHash(fileName);

		return findResource(type, nameHash);
	}

	void ResourceCache::RemoveResource(StringHash type, const std::string& name)
	{
		auto fileName = FileSystem::GetFileNameAndExtension(name);
		StringHash nameHash(fileName);
		auto* pExistingResource = findResource(type, nameHash);
		if (!pExistingResource)
			return;

		DV_DELETE(pExistingResource);
		m_ResourceGroups[type.GetValue()].erase(nameHash.GetValue());
	}

	void ResourceCache::RemoveSpecificTypeResources(StringHash type)
	{
		auto i = m_ResourceGroups.find(type.GetValue());
		if (i == m_ResourceGroups.end())
			return;

		for (auto j = i->second.begin(); j != i->second.end(); ++j)
			DV_DELETE(j->second);

		m_ResourceGroups.erase(i);
	}

	void ResourceCache::RemoveAllResources()
	{
		for (auto i = m_ResourceGroups.begin(); i != m_ResourceGroups.end(); ++i)
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				DV_DELETE(j->second);
			}
		}

		m_ResourceGroups.clear();
	}

	Resource* ResourceCache::findResource(StringHash type, StringHash nameHash)
	{
		auto i = m_ResourceGroups.find(type.GetValue());
		if (i == m_ResourceGroups.end())
			return nullptr;

		auto j = i->second.find(nameHash.GetValue());
		if (j == i->second.end())
			return nullptr;

		return j->second;
	}
}