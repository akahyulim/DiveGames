#pragma once
#include "Resource.h"
#include "Base/DiveCore.h"

namespace Dive
{
	// Ư�� ����� ������Ʈ �̸����� ������ �����ȴ�.
	// => ��� ������ �����ϰ� �Ϸ��� �ᱹ ini ���� � �����ؾ� �Ѵ�.
	// Assets�� Art�� Textures, Sprites, Models, Materials ������ ����������.
	// Assets�� Scenes�� Scene, Prefabs�� prefab�� ����ȴ�.
	// Editor�� ��� ���� �� Project�� Load�ϸ� ���� Asset���� ��� �޸𸮿� �ø���.
	// App�� ���SceneName.dat ��� ���� ������ �ʿ��ϴ�. �̸� ���� �ش� Scene�� �ʿ��� ��� Resource���� �ε��Ѵ�. 
	// �ش� ������ ���ҽ� ���ϵ��� ��η� �����Ǹ�, Editor�� ����°� �翬�ϴ�.
	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance()
		{
			static ResourceManager instance;
			return instance;
		}

		template<class T>
		T* Cache(const T* pResource)
		{
			if (pResource == nullptr)
				return nullptr;

			if (IsCached(pResource->GetName(), pResource->GetType()))
				return GetByName<T>(pResource->GetName());
			
			return dynamic_cast<T*>(m_Resources.emplace_back(const_cast<T*>(pResource)));
		}

		template<class T>
		T* Load(const std::string& filepath)
		{
			// ���� const std::filesystem::path& ������ ��������.

			// ���� ���� ����
			if (!std::filesystem::exists(filepath))
				return nullptr;

			// �̸��� �̾Ƴ���
			const std::string name = std::filesystem::path(filepath).stem().string();

			if (IsCached(name, Resource::TypeToEnum<T>()))
				return GetByName<T>(name);

			// Ÿ�Կ� ���� ��ü ����
			auto pNewResource = new T;
			// �ϴ� ���⿡�� �̸� ����
			pNewResource->SetName(name);
			
			// LoadFromFile override �Լ� ȣ��
			if (pNewResource == nullptr || !pNewResource->LoadFromFile(filepath))
			{
				return nullptr;
			}
			
			// Cache
			return Cache<T>(pNewResource);
		}

		template<class T>
		void Remove(T* pResource)
		{
			if (pResource == nullptr)
				return;

			if (!IsCached(pResource->GetName(), pResource->GetType()))
				return;

			auto it = m_Resources.begin();
			for (it; it != m_Resources.end();)
			{
				if ((*it)->GetInstanceID() == pResource->GetInstanceID())
				{
					DV_DELETE(*it);
					it = m_Resources.erase(it);
					return;
				}
				else
					it++;
			}
		}

		bool IsCached(const std::string& name, eResourceType type);
		template<class T>
		bool IsCached(const std::string& name)
		{
			return IsCached(name, Resource::TypeToEnum<T>());
		}
		
		Resource* GetByName(const std::string& name, eResourceType type);
		template<class T>
		T* GetByName(const std::string& name)
		{
			return dynamic_cast<T*>(GetByName(name, Resource::TypeToEnum<T>()));
		}
		
		void LoadResourcesFromAssetFolder();
		void SaveResourcesToFile(const std::string& filepath);
		void LoadResourcesFromFile(const std::string& filepath);

	private:
		ResourceManager();
		~ResourceManager();

	private:
		std::vector<Resource*> m_Resources;

		// assetŸ�Ժ��� direcoty�� �ٸ���.
	};
}