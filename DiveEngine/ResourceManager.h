#pragma once
#include "Resource.h"
#include <vector>

namespace dive
{
	// Ư�� ����� ������Ʈ �̸����� ������ �����ȴ�.
	// => ��� ������ �����ϰ� �Ϸ��� �ᱹ ini ���� � �����ؾ� �Ѵ�.
	// Assets�� Art�� Textures, Sprites, Models, Materials ������ ����������.
	// Assets�� Scenes�� Scene, Prefabs�� prefab�� ����ȴ�.
	// Editor�� ���� �� Project�� Load�ϸ� ���� Asset���� ��� �޸𸮿� �ø���.
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

			// �̹� cache �Ǿ��ٸ� �׳� ����

			return dynamic_cast<T*>(m_Resources.emplace_back(pResource));
		}

		template<class T>
		T* Load(const std::string& filepath)
		{
			// ���� ���� ����

			// ���ĸ�ź�� ��ü ���� ��
			// Resource::LoadFromFile()�� path�� �����ߴ�.
			// �׷��� �̴� Unity�� Load�ʹ� ������ �ٸ���. �Դٰ� Load�� Resource override�� �ƴϴ�.
			// ���������� ���� �� LoadFromFile()�� �´�.
			// Texture::Load()�δ� Engine Format�� ��� Data�� �ѱ� �� ����.
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
		std::vector<Resource*> m_Resources;	// shared_ptr��...?

		// assetŸ�Ժ��� direcoty�� �ٸ���.

		// importer��
	};
}