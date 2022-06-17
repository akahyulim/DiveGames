#pragma once
#include "Base/Base.h"
#include "Resource.h"
#include "Helper/FileSystem.h"
#include "Renderer/Graphics/Texture2D.h"
#include "Renderer/Material.h"

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
		T* Cache(T* pResource)
		{
			if (pResource == nullptr)
				return nullptr;

			auto name = pResource->GetName();
			if (HasResource<T>(name))
				return GetResource<T>(name);

			// 이 곳에서 save to file을 한 번 한다.
			// 이때문에 resource는 engine file path를 가져야 한다.
			// 그런데 어찌되었던, 최신 상태 파일은 SaveToDataFile()에서 다시 만들때 생성한다.

			return dynamic_cast<T*>(m_Resources.emplace_back(pResource));
		}

		// bug
		// 이미 생성된 리소스가 있을 경우 해당 객체를 리턴한다.
		// 텍스쳐의 경우 그냥 사용해도 문제가 없지만
		// 모델의 경우 새로운 게임 오브젝트를 생성하지 못하고 있다.
		// 스파르탄 코드를 참조한 것인데
		// 이 함수의 문제라기보단 모델을 추가하는 패널에서 수정해야 할 것 같다.
		template<class T>
		T* Load(const std::string& filepath)
		{
			if (!std::filesystem::exists(filepath))
			{
				DV_CORE_WARN("잘못된 경로를 전달받았습니다 - {:s}", filepath);
				return nullptr;
			}

			auto name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);

			if (HasResource<T>(name))
				return GetResource<T>(name);

			auto pCreatedResource = new T();
			pCreatedResource->SetName(name);
			if (!pCreatedResource->LoadFromFile(filepath))
			{
				DV_CORE_ERROR("파일 로드에 실패하였습니다 - {:s}", filepath);
				return nullptr;
			}

			return Cache<T>(pCreatedResource);
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