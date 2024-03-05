#include "DivePch.h"
#include "ResourceManager.h"

namespace Dive
{
	std::unordered_map<size_t, ResourceGroup> ResourceManager::s_ResourceGroups;

	bool ResourceManager::AddManualResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_CORE_ERROR("잘못된 리소스 포인터를 전달받았습니다.");
			return false;
		}

		auto& name = pResource->GetName();
		if(name.empty())
		{
			DV_CORE_ERROR("매뉴얼 리소스를 추가하기 위해선 리소스에 이름이 존재해야 합니다.");
			return false;
		}

		auto it = s_ResourceGroups[pResource->GetTypeHash()].find(pResource->GetNameHash());
		if (it != s_ResourceGroups[pResource->GetTypeHash()].end())
		{
			DV_CORE_ERROR("동일한 이름의 리소스({:s})가 이미 등록되어 있습니다.", name);
			return false;
		}

		s_ResourceGroups[pResource->GetTypeHash()].emplace(pResource->GetNameHash(), pResource);
		return true;
	}

	void ResourceManager::Shutdown()
	{
		auto it = s_ResourceGroups.begin();
		for (it; it != s_ResourceGroups.end(); it++)
		{
			for (auto& resourceGroup : it->second)
				DV_DELETE(resourceGroup.second);
		}
		s_ResourceGroups.clear();
	}

	Resource* ResourceManager::findResource(size_t typeHash, size_t nameHash)
	{
		auto it = s_ResourceGroups.find(typeHash);
		if (it != s_ResourceGroups.end())
		{
			auto resourceGroup = it->second.find(nameHash);
			if (resourceGroup != it->second.end())
				return resourceGroup->second;
		}

		return nullptr;
	}
}