#include "stdafx.h"
#include "ResourceManager.h"

namespace Dive
{
	std::unordered_map<size_t, std::vector<Resource*>> ResourceManager::s_Resources;
	std::filesystem::path ResourceManager::s_ResourceFolder = "Assets";

	void ResourceManager::Clear()
	{
		for (auto& [typeId, resources] : s_Resources)
		{
			for (auto* resource : resources)
			{
				DV_DELETE(resource);
			}
			resources.clear();
		}
		s_Resources.clear();
	}

	bool ResourceManager::IsCahed(InstanceID instanceID)
	{
		for (const auto& [typeId, resources] : s_Resources)
		{
			for (const auto* resource : resources)
			{
				if (resource->GetInstanceID() == instanceID)
				{
					return true;
				}
			}
		}
		return false;
	}

	INT32 ResourceManager::GetAllResourceCount()
	{
		INT32 count = 0;
		for (const auto& [typeId, resources] : s_Resources)
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
