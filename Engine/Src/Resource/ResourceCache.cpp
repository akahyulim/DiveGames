#include "divepch.h"
#include "ResourceCache.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "IO/FileSystem.h"

namespace Dive 
{
	ResourceCache::ResourceCache(Context* pContext)
		: Object(pContext)
	{
	}

	ResourceCache::~ResourceCache()
	{
	}

	// manual resource라고 구분해 놓았다.
	bool ResourceCache::AddResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_LOG_ENGINE_ERROR("잘못된 인자를 전달받았습니다.");
			return false;
		}

		m_ResourceGroups[pResource->GetType().Value()][pResource->GetNameHash().Value()] = pResource;

		return true;
	}

	// 이미 관리 중이라면 찾아서 리턴, 아니면 직접 생성 및 초기화한 후 저장하고 리턴.
	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		// nameHash 만들기
		// 원래 좀 더 복잡하다. 그리고 함수로 만들어 사용한다.
		// => Reosurce에도 NameHash가 있다. 이경우에는 SetName을 통해 만들었다.
		// 따라서 추후 아래 부분을 다듬어야 한다.
		auto sanitateName = GetInternalPath(name);
		sanitateName = GetFileNameAndExtension(sanitateName);
		sanitateName = StringTrim(sanitateName);
		StringHash nameHash(sanitateName);

		auto* pExistedResource = findResource(type, nameHash);
		if (pExistedResource)
			return pExistedResource;

		auto* pNewResource = new Resource(m_pContext);
		// name이 경로를 포함하지 않을 수 있다.
		// 그렇다면 name을 통해 경로를 덧붙여야 한다.
		if (!pNewResource->LoadFromFile(name))
			return nullptr;

		m_ResourceGroups[type.Value()][nameHash.Value()] = pNewResource;

		return pNewResource;
	}

	void ResourceCache::GetResources(StringHash type, std::vector<Resource*>& outResources)
	{
		outResources.clear();

		auto i = m_ResourceGroups.find(type.Value());
		if (i != m_ResourceGroups.end())
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				outResources.emplace_back(j->second);
			}
		}
	}

	// 원래 전부 bForce라는 전달인자를 가졌다.
	// 추후 UpdateResourceGroups() 등에서 확인을 거친 후 제거해야 할 수 있다.
	// 하지만 urho에는 그런 구문이 없었다.
	void ResourceCache::ReleaseResource(StringHash type, const std::string& name)
	{
		StringHash nameHash(name);
		auto* pExistedReosurce = findResource(type, nameHash);
		if (!pExistedReosurce)
			return;

		DV_DELETE(pExistedReosurce);
		m_ResourceGroups[type.Value()].erase(nameHash.Value());
	}

	void ResourceCache::ReleaseResources(StringHash type)
	{
		auto i = m_ResourceGroups.find(type.Value());
		if (i == m_ResourceGroups.end())
			return;

		for (auto j = i->second.begin(); j != i->second.end(); ++j)
			DV_DELETE(j->second);

		m_ResourceGroups.erase(i);
	}

	void ResourceCache::ReleaseAllResources()
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
		auto i = m_ResourceGroups.find(type.Value());
		if (i == m_ResourceGroups.end())
			return nullptr;

		auto j = i->second.find(nameHash.Value());
		if (j == i->second.end())
			return nullptr;

		return j->second;
	}
}