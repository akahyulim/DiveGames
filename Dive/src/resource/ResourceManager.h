#pragma once
#include "Resource.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		static void Shutdown();

		template<class T>
		static std::shared_ptr<T> Load(const std::string& filepath);

		template<class T>
		static std::shared_ptr<T> GetByName(const std::string& name);

		template<class T>
		static std::shared_ptr<T> GetByPath(const std::string& filepath);

		template<class T>
		static std::vector<std::shared_ptr<T>> GetByType();

		template<class T>
		static uint32_t GetResourceCount();
		static uint32_t GetAllResourceCount();
		
		template<class T>
		static bool IsCached(const std::string& filepath);
		static bool IsCached(uint64_t id);
		
	private:
		static std::mutex s_Mutex;
		static std::unordered_map<uint32_t, std::vector<std::shared_ptr<Resource>>> s_Resources;
	};

	template<class T>
	static std::shared_ptr<T> ResourceManager::Load(const std::string& filepath)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			for (const auto& pResource : it->second)
			{
				if (pResource->GetFilepath() == filepath)
				{
					return std::dynamic_pointer_cast<T>(pResource);
				}
			}
		}

		auto pNewResource = std::make_shared<T>();
		pNewResource->LoadFromFile(filepath);

		s_Resources[T::GetTypeHashStatic()].push_back(std::static_pointer_cast<Resource>(pNewResource));

		return pNewResource;
	}

	template<class T>
	static std::shared_ptr<T> ResourceManager::GetByName(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			for (const auto& pResource : it->second)
			{
				if (pResource->GetName() == name)
					return std::dynamic_pointer_cast<T>(pResource);
			}
		}

		return nullptr;
	}

	template<class T>
	static std::shared_ptr<T> ResourceManager::GetByPath(const std::string& filepath)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			for (const auto& pResource : it->second)
			{
				if (pResource->GetFilepath() == filepath)
					return std::dynamic_pointer_cast<T>(pResource);
			}
		}

		return nullptr;
	}

	template<class T>
	static std::vector<std::shared_ptr<T>> ResourceManager::GetByType()
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			std::vector<std::shared_ptr<T>> resources;
			resources.reserve(it->second.size());
			for (const auto& pResource : it->second)
			{
				resources.push_back(std::dynamic_pointer_cast<T>(pResource));
			}

			return resources;
		}

		return {};
	}

	template<class T>
	static uint32_t ResourceManager::GetResourceCount()
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			return static_cast<uint32_t>(it->second.size());
		}

		return 0;
	}

	template<class T>
	static bool ResourceManager::IsCached(const std::string& filepath)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto it = s_Resources.find(T::GetTypeHashStatic());
		if (it != s_Resources.end())
		{
			for (const auto& pResource : it->second)
			{
				if (pResource->GetFilepath() == filepath)
					return true;
			}
		}

		return false;
	}
}