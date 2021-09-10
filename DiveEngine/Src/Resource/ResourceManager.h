#pragma once
#include "Resource.h"
#include "../DiveCore.h"
#include "../FileSystemHelper.h"
#include <vector>
#include <algorithm>

namespace dive
{
	// 특정 경로의 프로젝트 이름으로 폴더가 생성된다.
	// => 경로 변경이 가능하게 하려면 결국 ini 파일 등에 저장해야 한다.
	// Assets의 Art에 Textures, Sprites, Models, Materials 등으로 나누어진다.
	// Assets의 Scenes에 Scene, Prefabs에 prefab이 저장된다.
	// Editor는 실행 후 Project를 Load하면 관련 Asset들을 모두 메모리에 올린다.
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
			// 파일 존재 여부
			if (!FileSystemHelper::FileExists(filepath))
				return nullptr;

			// 이름만 뽑아내기
			const std::string name = FileSystemHelper::GetFilenameWithoutExtension(filepath);

			if (IsCached(name, Resource::TypeToEnum<T>()))
				return GetByName<T>(name);

			// 타입에 맞춰 객체 생성
			auto pNewResource = new T;
			// 일단 여기에서 이름 저장
			pNewResource->SetName(name);
			
			// LoadFromFile override 함수 호출
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
		

	private:
		ResourceManager();
		~ResourceManager();

	private:
		std::vector<Resource*> m_Resources;

		// asset타입별로 direcoty가 다르다.
	};
}