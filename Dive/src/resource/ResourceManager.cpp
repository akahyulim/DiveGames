#include "stdafx.h"
#include "ResourceManager.h"

namespace Dive
{
	std::unordered_map<eResourceType, std::vector<std::shared_ptr<Resource>>> ResourceManager::s_Resources;
	std::filesystem::path ResourceManager::s_ResourceFolder = "Assets";

	void ResourceManager::Clear()
	{
		// shared_ptr 대신 unique_ptr, raw pointer 조합이 맞는 것 같다.
		s_Resources.clear();

		DV_LOG(ResourceManager, info, "클리어 완료");
	}

	bool ResourceManager::IsCahed(UINT64 instanceID)
	{
		for (const auto& [type, resources] : s_Resources)
		{
			for (auto& resource : resources)
			{
				if (resource->GetInstanceID() == instanceID)
					return true;
			}
		}
		return false;
	}

	INT32 ResourceManager::GetAllResourceCount()
	{
		INT32 count = 0;
		for (const auto& [type, resources] : s_Resources)
		{
			count += static_cast<INT32>(resources.size());
		}
		return count;
	}

	void ResourceManager::SetResourceFolder(const std::filesystem::path& path)
	{
		if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
		{
			DV_LOG(ResourceManager, err, "잘못된 에셋 폴더 경로 전달: {}", path.string());
			return;
		}

		s_ResourceFolder = path;
		std::filesystem::current_path(s_ResourceFolder);
	}
}
