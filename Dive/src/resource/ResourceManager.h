#pragma once
#include "Resource.h"
#include "Common.h"
#include "core/FileUtils.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		static bool Initialize();
		static void Clear();

		template<class T>
		static T* Load(const std::filesystem::path& filepath)
		{
			auto& vec = s_resources[T::GetType()];
			for (const auto& resource : vec)
			{
				if (resource->GetFilepath() == filepath)
					return dynamic_cast<T*>(resource.get());
			}

			auto resource = std::make_unique<T>();
			if (!resource->LoadFromFile(filepath))
			{
				DV_LOG(ResourceManager, err, "[::Load] Resource::LoadFromFile() 실패");
				return nullptr;
			}

			T* raw = resource.get();
			vec.push_back(std::move(resource));
			return raw;
		}

		template<class T>
		static T* GetByName(const std::string& name)
		{
			auto it = s_resources.find(T::GetType());
			if (it != s_resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetName() == name)
						return dynamic_cast<T*>(resource.get());
				}
			}
			return nullptr;
		}

		template<class T>
		static T* GetOrLoadByPath(const std::filesystem::path& filepath)
		{
			auto& vec = s_resources[T::GetType()];
			for (const auto& resource : vec)
			{
				if (resource->GetFilepath() == filepath)
					return dynamic_cast<T*>(resource.get());
			}

			return Load<T>(filepath);
		}

		template<class T>
		static std::vector<T*> GetByType()
		{
			auto it = s_resources.find(T::GetType());
			if (it != s_resources.end())
			{
				std::vector<T*> resources;
				resources.reserve(it->second.size());
				for (const auto& resource : it->second)
					resources.push_back(dynamic_cast<T*>(resource.get()));
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

		static bool IsCahed(UINT64 instanceID);

		template<class T>
		static uint32_t GetResourceCount()
		{
			auto it = s_resources.find(T::GetType());
			return it != s_resources.end() ? static_cast<uint32_t>(it->second.size()) : 0;
		}

		template<class T>
		static void RegisterResource(std::unique_ptr<T> resource)
		{
			if (!resource || IsCached<T>(resource->GetFilepath()))
				return;

			s_resources[T::GetType()].push_back(std::move(resource));
		}

		static uint32_t GetAllResourceCount();

	private:
		static std::unordered_map<eResourceType, std::vector<std::unique_ptr<Resource>>> s_resources;
	};
}
