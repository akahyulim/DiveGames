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

	// manual resource��� ������ ���Ҵ�.
	bool ResourceCache::AddResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_LOG_ENGINE_ERROR("�߸��� ���ڸ� ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_ResourceGroups[pResource->GetType().Value()][pResource->GetNameHash().Value()] = pResource;

		return true;
	}

	// ���� ���� bForce��� �������ڸ� ������.
	// ���� UpdateResourceGroups() ��� Ȯ���� ��ģ �� �����ؾ� �� �� �ִ�.
	// ������ urho���� �׷� ������ ������.
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

	// name�� ����, ��� ���, ���� + �̸� + �ͽ��ټ�(= Reosurce�� �̸�) �� �پ��ϰ� �� �� �ִ�.
	// �ݸ� �� �Լ� ���ο����� ������ ��ο� Resource�� �̸����� ������ ���ȴ�.
	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		// nameHash
		// �⺻������ ���ҽ� Ÿ�� ���� + �̸� + �ͽ��ټ����� �ٵ������.
		auto sanitateName = FileSystem::GetInternalPath(name);
		sanitateName = FileSystem::GetFileNameAndExtension(sanitateName);
		sanitateName = FileSystem::StringTrim(sanitateName);
		StringHash nameHash(sanitateName);

		auto* pExistedResource = findResource(type, nameHash);
		if (pExistedResource)
			return pExistedResource;

		auto* pNewResource = new Resource(m_pContext);
		// �׳� name���δ� �ȵ� �� �ִ�.
		// ������ urho�� File�� ���� �� ���� ���ҽ� ���� + name���� ���� ���� ���θ� Ȯ���Ѵ�.
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