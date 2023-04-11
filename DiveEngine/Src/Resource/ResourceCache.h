#pragma once
#include "Resource.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	class ModelImporter;

	class ResourceCache
	{
	public:
		static bool Initialize();
		static void Shutdown();

		template<class T>
		static bool AddManualResource(T* pResource)
		{
			DV_ASSERT(pResource);

			if (pResource->GetName().empty())
			{
				DV_CORE_WARN("이름이 존재하지 않는 리소스를 매뉴얼 리소스로 저장할 수 없습니다.");
				return false;
			}

			return Cache<T>(pResource);
		}

		template<class T>
		static T* Cache(T* pResource)
		{
			if (!pResource)
				return nullptr;

			if (IsCached<T>(pResource->GetName()))
			{
				DV_CORE_WARN("이미 저장된 리소스({0:s} : {1:d})의 캐시를 시도하였습니다.", pResource->GetName(), pResource->GetID());
				return pResource;
			}

			auto id = getFreeID();
			if (id == 0)
			{
				DV_CORE_ERROR("더이상 리소스 객체를 캐시할 수 없습니다.");
				return pResource;
			}

			m_Resources[id] = static_cast<Resource*>(pResource);
			pResource->SetID(id);

			DV_CORE_DEBUG("리소스({0:s} - {1:d})를 캐시하였습니다", pResource->GetName(), pResource->GetID());

			return pResource;
		}

		template<class T>
		static void Remove(T* pResource)
		{
			if (!pResource)
				return;

			RemoveByName<T>(pResource->GetName());
		}

		static void RemoveByID(uint64_t id);

		template<class T>
		static void RemoveByName(const std::string& name)
		{
			for (auto& resource : m_Resources)
			{
				if (resource.second->GetResourceType() == Resource::TypeToEnum<T>() &&
					resource.second->GetName() == name)
				{
					DV_DELETE(resource.second);
					DV_CORE_DEBUG("{:s} 리소스 객체를 캐시에서 제거하였습니다.", name);
					return;
				}
			}
		}

		template<class T>
		static bool IsCached(const std::string& name)
		{
			for (auto& resource : m_Resources)
			{
				if (resource.second->GetResourceType() == Resource::TypeToEnum<T>() &&
					resource.second->GetName() == name)
				{
					DV_CORE_DEBUG("이미 캐시된 리소스({0:s} : {1:d})입니다.", resource.second->GetName(), resource.second->GetID());
					return true;
				}
			}

			return false;
		}

		static bool IsCachedByID(uint64_t id);

		template<class T>
		static T* LoadFromFile(const std::string& filePath)
		{
			if (!FileSystem::FileExists(filePath))
			{
				DV_CORE_ERROR("파일({:s})(이)가 존재하지 않습니다.", filePath);
				return nullptr;
			}

			auto pNewResource = new T;
			if (!pNewResource->LoadFromFile(filePath))
			{
				DV_DELETE(pNewResource);
				return nullptr;
			}

			return Cache<T>(pNewResource);
		}

		template<class T>
		static T* GetResourceByID(uint64_t id)
		{
			auto it = m_Resources.find(id);
			return it != m_Resources.end() ? dynamic_cast<T*>(it->second) : nullptr;
		}

		template<class T>
		static T* GetResourceByName(const std::string& name)
		{
			for (auto& resource : m_Resources)
			{
				if (resource.second->GetResourceType() == Resource::TypeToEnum<T>() &&
					resource.second->GetName() == name)
				{
					return dynamic_cast<T*>(resource.second);
				}
			}

			return nullptr;
		}

		template<class T>
		static std::vector<T*> GetResourcesByType()
		{
			std::vector<T*> resources;

			for (auto& resource : m_Resources)
			{
				if (resource.second->GetResourceType() == Resource::TypeToEnum<T>())
					resources.emplace_back(resource.second);
			}

			return resources;
		}

		template<class T>
		static T* GetResourceByPath(const std::string& filePath)
		{
			for (auto& resource : m_Resources)
			{
				if (resource.second->GetResourceType() == Resource::TypeToEnum<T>())
				{
					if (resource.second->GetFilePath() == filePath)
						return static_cast<T*>(resource.second);
				}
			}

			return LoadFromFile<T>(filePath);
		}

		static ModelImporter* GetModelImporter();

	private:
		static uint64_t getFreeID();

	private:
		static uint64_t m_CurResourceID;
		static std::unordered_map<uint64_t, Resource*> m_Resources;
	};
}