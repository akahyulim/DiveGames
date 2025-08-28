#pragma once
#include "Resource.h"
#include "Common.h"
#include "core/FileUtils.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		template<class T>
		static std::shared_ptr<T> GetOrLoad(const std::filesystem::path& filepath)
		{
			auto& vec = s_resources[T::GetType()];
			for (const auto& resource : vec)
			{
				if (resource->GetFilepath() == filepath)
					return std::static_pointer_cast<T>(resource);
			}

			auto resource = std::make_shared<T>();
			if (!resource->LoadFromFile(filepath))
			{
				DV_LOG(ResourceManager, err, "[::Load] {} 로딩 실패", filepath.string());
				return nullptr;
			}

			vec.push_back(resource);
			return resource;
		}
		
		static void Register(std::shared_ptr<Resource> resource)
		{
			if (IsCached(resource))
				return;

			s_resources[resource->GetType()].push_back(resource);
		}

		static void Clear();
		
		template<class T>
		static std::shared_ptr<T> GetByName(const std::string& name)
		{
			auto it = s_resources.find(T::GetType());
			if (it != s_resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetName() == name)
						return std::static_pointer_cast<T>(resource);
				}
			}
			return nullptr;
		}

		template<class T>
		static std::vector<std::shared_ptr<T>> GetByType()
		{
			auto it = s_resources.find(T::GetType());
			if (it != s_resources.end())
			{
				std::vector<std::shared_ptr<T>> resources;
				resources.reserve(it->second.size());
				for (const auto& resource : it->second)
					resources.push_back(std::static_pointer_cast<T>(resource));

				return resources;
			}
			return {};
		}

		template<class T>
		static bool IsCached(const std::filesystem::path& filepath)
		{
			auto it = s_resources.find(T::GetType());
			if (it != s_resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
						return true;
				}
			}
			return false;
		}

		static bool IsCached(std::shared_ptr<Resource> resource);
		static bool IsCached(uint64_t instanceID);

		template<class T>
		static uint32_t GetResourceCount()
		{
			auto it = s_resources.find(T::GetType());
			return it != s_resources.end() ? static_cast<uint32_t>(it->second.size()) : 0;
		}

		static uint32_t GetAllResourceCount();

	private:
		static std::unordered_map<eResourceType, std::vector<std::shared_ptr<Resource>>> s_resources;
	};
}
