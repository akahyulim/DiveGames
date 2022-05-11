#pragma once
#include "Base/Base.h"
#include "Resource.h"
#include "Helper/FileSystem.h"
#include "Renderer/Graphics/Texture.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance()
		{
			static ResourceManager inst;
			return inst;
		}

		void Clear();

		template<class T>
		T* Load(const std::string& filepath)
		{
			if (!std::filesystem::exists(filepath))
			{
				DV_CORE_WARN("잘못된 경로를 전달받았습니다 - {:s}", filepath);
				return nullptr;
			}

			auto name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);

			// 추가 로드를 시도해도 이미 만들어져 있는 것을 리턴한다...
			// 따라서 복사한 후 리턴하도록 만들어야 한다.
			if (HasResource<T>(name))
				return GetResource<T>(name);

			auto pCachedResource = new T(name);
			if (!pCachedResource->LoadFromFile(filepath))
			{
				DV_CORE_ERROR("파일 로드에 실패하였습니다 - {:s}", filepath);
				return nullptr;
			}

			m_Resources.emplace_back(pCachedResource);

			return pCachedResource;
		}

		bool HasResource(const std::string& name, const eResourceType type);
		template<class T>
		bool HasResource(const std::string& name)
		{
			return HasResource(name, T::GetStaticType());
		}
		bool HasResourceByID(unsigned long long id);

		template<class T>
		void Remove(Resource* pResource)
		{
			if (pResource == nullptr)
				return;

			if (!HasResourceByID(pResource->GetInstanceID()))
				return;

			for (auto it = m_Resources.begin(); it != m_Resources.end(); it++)
			{
				if ((*it)->GetInstanceID() == pResource->GetInstanceID())
				{
					DV_DELETE(*it);
					m_Resources.erase(it);
					return;
				}
			}
		}
		
		Resource* GetResource(const std::string& name, const eResourceType type);
		template<class T>
		T* GetResource(const std::string& name)
		{
			return dynamic_cast<T*>(GetResource(name, T::GetStaticType()));
		}
		Resource* GetResourceByID(unsigned long long id);

		unsigned int GetResourceCount() const { return static_cast<unsigned int>(m_Resources.size()); }

		bool SaveToDataFile();
		bool LoadFromDataFile();

	private:
		std::vector<Resource*> m_Resources;
	};
}