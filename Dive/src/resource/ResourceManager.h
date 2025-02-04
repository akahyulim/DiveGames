
#pragma once
#include "Resource.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"

namespace Dive
{
	extern class ResourceManager* GResourceManager;

	class ResourceManager
	{
	public:
		static ResourceManager* GetInstance()
		{
			std::call_once(s_OnceFlag, []()
				{
					s_pInstance = new ResourceManager;
				});
			return s_pInstance;
		}

		void Shutdown();

		template<class T>
		std::shared_ptr<T> Load(const std::filesystem::path& filepath);

		template<class T>
		std::shared_ptr<T> GetByName(const std::string& name);

		template<class T>
		std::shared_ptr<T> GetByPath(const std::filesystem::path& filepath);

		template<class T>
		std::vector<std::shared_ptr<T>> GetByType();

		template<class T>
		uint32_t GetResourceCount();
		uint32_t GetAllResourceCount();
		
		template<class T>
		bool IsCached(const std::string& filepath);
		bool IsCached(uint64_t id);
		
	private:
		ResourceManager() = default;
		~ResourceManager() = default;

	private:
		static ResourceManager* s_pInstance;
		static std::once_flag s_OnceFlag;

		std::mutex m_Mutex;
		std::unordered_map<size_t, std::vector<std::shared_ptr<Resource>>> m_Resources;
	};

	template<class T>
	std::shared_ptr<T> ResourceManager::Load(const std::filesystem::path& filepath)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
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

		m_Resources[T::GetTypeHashStatic()].push_back(std::static_pointer_cast<Resource>(pNewResource));

		return pNewResource;
	}

	template<class T>
	std::shared_ptr<T> ResourceManager::GetByName(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
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
	std::shared_ptr<T> ResourceManager::GetByPath(const std::filesystem::path& filepath)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
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
	std::vector<std::shared_ptr<T>> ResourceManager::GetByType()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
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
	uint32_t ResourceManager::GetResourceCount()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
		{
			return static_cast<uint32_t>(it->second.size());
		}

		return 0;
	}

	template<class T>
	bool ResourceManager::IsCached(const std::string& filepath)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto it = m_Resources.find(T::GetTypeHashStatic());
		if (it != m_Resources.end())
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