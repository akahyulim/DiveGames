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
			DV_ENGINE_WARN("잘못된 리소스 포인터를 전달받았습니다.");
			return false;
		}

		auto& name = pResource->GetName();
		if (name.empty())
		{
			DV_ENGINE_WARN("매뉴얼 리소스를 추가하기 위해선 리소스에 이름이 존재해야 합니다.");
			return false;
		}

		// sponza에서 동일한 이름때문에 문제가 발생했다. 
		// 일단 ModelLoader쪽의 AddManualResource()를 모두 주석처리 했다.
		auto it = m_ResourceGroups[pResource->GetTypeHash()].find(pResource->GetNameHash());
		if (it != m_ResourceGroups[pResource->GetTypeHash()].end())
		{
			DV_ENGINE_WARN("동일한 이름의 리소스({0:s}: {1:s})가 이미 등록되어 있습니다.", pResource->GetTypeName(), name);
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

		DV_ENGINE_TRACE("리소스 매니져 셧다운에 성공하였습니다.");
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