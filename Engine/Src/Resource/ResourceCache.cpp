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

	// �̹� ���� ���̶�� ã�Ƽ� ����, �ƴϸ� ���� ���� �� �ʱ�ȭ�� �� �����ϰ� ����.
	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		// nameHash �����
		// ���� �� �� �����ϴ�. �׸��� �Լ��� ����� ����Ѵ�.
		// => Reosurce���� NameHash�� �ִ�. �̰�쿡�� SetName�� ���� �������.
		// ���� ���� �Ʒ� �κ��� �ٵ��� �Ѵ�.
		auto sanitateName = GetInternalPath(name);
		sanitateName = GetFileNameAndExtension(sanitateName);
		sanitateName = StringTrim(sanitateName);
		StringHash nameHash(sanitateName);

		auto* pExistedResource = findResource(type, nameHash);
		if (pExistedResource)
			return pExistedResource;

		auto* pNewResource = new Resource(m_pContext);
		// name�� ��θ� �������� ���� �� �ִ�.
		// �׷��ٸ� name�� ���� ��θ� ���ٿ��� �Ѵ�.
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

	// ���� ���� bForce��� �������ڸ� ������.
	// ���� UpdateResourceGroups() ��� Ȯ���� ��ģ �� �����ؾ� �� �� �ִ�.
	// ������ urho���� �׷� ������ ������.
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