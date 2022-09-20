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

	// 원래 전부 bForce라는 전달인자를 가졌다.
	// 추후 UpdateResourceGroups() 등에서 확인을 거친 후 제거해야 할 수 있다.
	// 하지만 urho에는 그런 구문이 없었다.
	void ResourceCache::RemoveResource(StringHash type, const std::string& name)
	{
		StringHash nameHash(name);
		auto* pExistedReosurce = findResource(type, nameHash);
		if (!pExistedReosurce)
			return;

		DV_DELETE(pExistedReosurce);
		m_ResourceGroups[type.Value()].erase(nameHash.Value());
	}

	void ResourceCache::RemoveResources(StringHash type)
	{
		auto i = m_ResourceGroups.find(type.Value());
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

	bool ResourceCache::AddResourceDir(const std::string& pathName)
	{
		if(pathName.empty())
			return false;

		auto fixedPath = Dive::FileSystem::AddTrailingSlash(pathName);
		if (!Dive::FileSystem::IsAbsolutePath(fixedPath))
			fixedPath = Dive::FileSystem::GetCurrentDir() + fixedPath;

		auto it = std::find(m_ResourceDirs.begin(), m_ResourceDirs.end(), fixedPath);
		if (m_ResourceDirs.empty() || it == m_ResourceDirs.end())
			m_ResourceDirs.emplace_back(fixedPath);

		return true;
	}

	void ResourceCache::RemoveResourceDir(const std::string& pathName)
	{
		if (pathName.empty())
			return;

		auto fixedPath = Dive::FileSystem::AddTrailingSlash(pathName);
		if (!Dive::FileSystem::IsAbsolutePath(fixedPath))
			fixedPath = Dive::FileSystem::GetCurrentDir() + fixedPath;

		auto it = m_ResourceDirs.begin();
		for (it; it != m_ResourceDirs.end();)
		{
			if ((*it) == fixedPath)
				it = m_ResourceDirs.erase(it);
			else
				++it;
		}
	}

	// name은 절대, 상대 경로, 폴더 + 이름 + 익스텐션(= Reosurce의 이름) 등 다양하게 올 수 있다.
	// 반면 이 함수 내부에서는 파일의 경로와 Resource의 이름으로 나뉘어 사용된다.
	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		// nameHash
		// 기본적으로 리소스 타입 폴더 + 이름 + 익스텐션으로 다듬어진다.
		auto sanitateName = FileSystem::GetInternalPath(name);
		sanitateName = FileSystem::GetFileNameAndExtension(sanitateName);
		sanitateName = FileSystem::StringTrim(sanitateName);
		StringHash nameHash(sanitateName);

		auto* pExistedResource = findResource(type, nameHash);
		if (pExistedResource)
			return pExistedResource;

		auto* pNewResource = new Resource(m_pContext);
		// 그냥 name으로는 안될 수 있다.
		// 실제로 urho는 File을 만들 때 각종 리소스 폴더 + name으로 파일 존재 여부를 확인한다.
		if (!pNewResource->LoadFromFile(name))
			return nullptr;

		pNewResource->SetName(sanitateName);

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