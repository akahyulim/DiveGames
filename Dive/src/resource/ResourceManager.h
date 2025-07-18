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
		static std::shared_ptr<T> Load(const std::filesystem::path& filepath)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
						return std::static_pointer_cast<T>(resource);
				}
			}
			auto resource = std::make_shared<T>();
			resource->LoadFromFile(filepath);
			s_Resources[T::GetType()].push_back(resource);
			return resource;
		}

		template<class T>
		static std::shared_ptr<T> GetByName(const std::string& name)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetName() == name)
						return std::dynamic_pointer_cast<T>(resource);
				}
			}
			return nullptr;
		}

		template<class T>
		static std::shared_ptr<T> GetByPath(const std::filesystem::path& filepath)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
			if (it != s_Resources.end())
			{
				for (const auto& resource : it->second)
				{
					if (resource->GetFilepath() == filepath)
					{
						return static_cast<std::shared_ptr<T>>(resource);
					}
				}
			}
			return nullptr;
		}

		template<class T>
		static std::vector<std::shared_ptr<T>> GetByType()
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
			if (it != s_Resources.end())
			{
				std::vector<std::shared_ptr<T>> resources;
				resources.reserve(it->second.size());
				for (const auto& resource : it->second)
				{
					resources.push_back(static_cast<std::shared_ptr<T>>(resource));
				}
				return resources;
			}
			return {};
		}

		template<class T>
		static bool IsCached(const std::string& filepath)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
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

		static bool IsCahed(UINT64 instanceID);

		template<class T>
		static uint32_t GetResourceCount()
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			auto it = s_Resources.find(T::GetType());
			if (it != s_Resources.end())
			{
				return static_cast<uint32_t>(it->second.size());
			}
			return 0;
		}

		template<class T>
		static void RegisterResource(std::shared_ptr<T> resource)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			if (resource)
			{
				s_Resources[T::GetType()].push_back(resource);
				resource->SetFilepath(s_ResourceFolder / resource->GetName());
			}
		}

		static uint32_t GetAllResourceCount();

		static std::string GetResourceFolder() { return s_ResourceFolder.string(); }
		static void SetResourceFolder(const std::filesystem::path& path);

	private:
		static std::unordered_map<eResourceType, std::vector<std::shared_ptr<Resource>>> s_Resources;
		static std::filesystem::path s_ResourceFolder;
	};
}
