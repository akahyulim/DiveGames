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
				DV_CORE_WARN("�̸��� �������� �ʴ� ���ҽ��� �Ŵ��� ���ҽ��� ������ �� �����ϴ�.");
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
				DV_CORE_WARN("�̹� ����� ���ҽ�({0:s} : {1:d})�� ĳ�ø� �õ��Ͽ����ϴ�.", pResource->GetName(), pResource->GetID());
				return pResource;
			}

			auto id = getFreeID();
			if (id == 0)
			{
				DV_CORE_ERROR("���̻� ���ҽ� ��ü�� ĳ���� �� �����ϴ�.");
				return pResource;
			}

			m_Resources[id] = static_cast<Resource*>(pResource);
			pResource->SetID(id);

			DV_CORE_DEBUG("���ҽ�({0:s} - {1:d})�� ĳ���Ͽ����ϴ�", pResource->GetName(), pResource->GetID());

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
					DV_CORE_DEBUG("{:s} ���ҽ� ��ü�� ĳ�ÿ��� �����Ͽ����ϴ�.", name);
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
					DV_CORE_DEBUG("�̹� ĳ�õ� ���ҽ�({0:s} : {1:d})�Դϴ�.", resource.second->GetName(), resource.second->GetID());
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
				DV_CORE_ERROR("����({:s})(��)�� �������� �ʽ��ϴ�.", filePath);
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