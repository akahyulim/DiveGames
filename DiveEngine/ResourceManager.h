#pragma once
#include "Resource.h"
#include <vector>

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

			// 이미 cache 되었다면 그냥 리턴

			return dynamic_cast<T*>(m_Resources.emplace_back(pResource));
		}

		template<class T>
		T* Load(const std::string& filepath)
		{
			// 파일 존재 여부

			// 스파르탄은 객체 생성 후
			// Resource::LoadFromFile()에 path를 전달했다.
			// 그런데 이는 Unity의 Load와는 성격이 다르다. 게다가 Load는 Resource override도 아니다.
			// 직관적으로 봤을 때 LoadFromFile()이 맞다.
			// Texture::Load()로는 Engine Format읠 모든 Data를 넘길 수 없다.
		}

		bool IsCached(const std::string& name, eResourceType type);

		template<class T>
		T* GetByName(const std::string& name)
		{

		}

		

	private:
		ResourceManager();
		~ResourceManager();

	private:
		std::vector<Resource*> m_Resources;	// shared_ptr로...?

		// asset타입별로 direcoty가 다르다.

		// importer들
	};
}