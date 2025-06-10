#pragma once
#include "Resource.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		static void Clear();

		template<class T>
		static T* Load(const std::filesystem::path& filepath)
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
					{
						return static_cast<T*>(resource);
					}
				}
			}
			auto resource = new T();
			resource->LoadFromFile(filepath);
			s_Resources[T::GetTypeHashStatic()].push_back(resource);
			return resource;
		}

		template<class T>
		static T* GetByName(const std::string& name)
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetName() == name)
					{
						return static_cast<T*>(resource);
					}
				}
			}
			return nullptr;
		}

		template<class T>
		static T* GetByPath(const std::filesystem::path& filepath)
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
					{
						return static_cast<T*>(resource);
					}
				}
			}
			return nullptr;
		}

		template<class T>
		static std::vector<T*> GetByType()
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				std::vector<T*> resources;
				resources.reserve(it->second.size());
				for (const auto& resource : it->second)
				{
					resources.push_back(static_cast<T*>(resource));
				}
				return resources;
			}
			return {};
		}

		template<class T>
		static bool IsCached(const std::string& filepath)
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
					{
						return true;
					}
				}
			}
			return false;
		}

		static bool IsCahed(InstanceID instanceID);

		template<class T>
		static INT32 GetResourceCount()
		{
			auto it = s_Resources.find(T::GetTypeHashStatic());
			if (it != s_Resources.end())
			{
				return static_cast<INT32>(it->second.size());
			}
			return 0;
		}

		static INT32 GetAllResourceCount();

		static std::string GetResourceFolder() { return s_ResourceFolder.string(); }
		static void SetResourceFolder(const std::filesystem::path& path);

	private:
		static std::unordered_map<size_t, std::vector<Resource*>> s_Resources;
		static std::filesystem::path s_ResourceFolder;
	};
}
