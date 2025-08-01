#include "stdafx.h"
#include "ResourceManager.h"

namespace Dive
{
	std::unordered_map<eResourceType, std::vector<std::shared_ptr<Resource>>> ResourceManager::s_resources;
	std::filesystem::path ResourceManager::s_resourceFolder = "Assets";

	void ResourceManager::Clear()
	{
		// shared_ptr 대신 unique_ptr, raw pointer 조합이 맞는 것 같다.
		s_resources.clear();

		DV_LOG(ResourceManager, info, "클리어 완료");
	}

	bool ResourceManager::IsCahed(UINT64 instanceID)
	{
		for (const auto& [type, resources] : s_resources)
		{
			for (auto& resource : resources)
			{
				if (resource->GetInstanceID() == instanceID)
					return true;
			}
		}
		return false;
	}

	uint32_t ResourceManager::GetAllResourceCount()
	{
		uint32_t count = 0;
		for (const auto& [type, resources] : s_resources)
		{
			count += static_cast<uint32_t>(resources.size());
		}
		return count;
	}

	void ResourceManager::SetResourceFolder(const std::filesystem::path& path)
	{
		if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
		{
			DV_LOG(ResourceManager, err, "[::SetResourceFolder] 잘못된 에셋 폴더 경로 전달: {}", path.string());
			return;
		}

		s_resourceFolder = path;
		std::filesystem::current_path(s_resourceFolder);
	}
}
