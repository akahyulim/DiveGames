#include "DivePch.h"
#include "ResourceManager.h"

namespace Dive
{
	ResourceManager* ResourceManager::s_pInstance = nullptr;

	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
		Shutdown();
	}

	bool ResourceManager::AddManualResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_ENGINE_WARN("�߸��� ���ҽ� �����͸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		auto& name = pResource->GetName();
		if (name.empty())
		{
			DV_ENGINE_WARN("�Ŵ��� ���ҽ��� �߰��ϱ� ���ؼ� ���ҽ��� �̸��� �����ؾ� �մϴ�.");
			return false;
		}

		// sponza���� ������ �̸������� ������ �߻��ߴ�. 
		// �ϴ� ModelLoader���� AddManualResource()�� ��� �ּ�ó�� �ߴ�.
		auto it = m_ResourceGroups[pResource->GetTypeHash()].find(pResource->GetNameHash());
		if (it != m_ResourceGroups[pResource->GetTypeHash()].end())
		{
			DV_ENGINE_WARN("������ �̸��� ���ҽ�({0:s}: {1:s})�� �̹� ��ϵǾ� �ֽ��ϴ�.", pResource->GetTypeName(), name);
			return false;
		}

		m_ResourceGroups[pResource->GetTypeHash()].emplace(pResource->GetNameHash(), pResource);

		return true;
	}

	void ResourceManager::Shutdown()
	{
		auto it = m_ResourceGroups.begin();
		for (it; it != m_ResourceGroups.end(); it++)
		{
			for (auto& resourceGroup : it->second)
				DV_DELETE(resourceGroup.second);
		}
		m_ResourceGroups.clear();

		DV_ENGINE_TRACE("���ҽ� �Ŵ��� �˴ٿ �����Ͽ����ϴ�.");
	}

	Resource* ResourceManager::findResource(size_t typeHash, size_t nameHash)
	{
		auto it = m_ResourceGroups.find(typeHash);
		if (it != m_ResourceGroups.end())
		{
			auto resourceGroup = it->second.find(nameHash);
			if (resourceGroup != it->second.end())
				return resourceGroup->second;
		}

		return nullptr;
	}

	ResourceManager* GetResourceManager()
	{
		return ResourceManager::GetInstance();
	}
}