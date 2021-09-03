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
		ResourceManager();
		~ResourceManager();

		template<class T>
		T* Load(const std::string& filepath)
		{
			// ���� ���� ����

			// ���� ���� ����

			// ���ĸ�ź�� ��ü ���� ��
			// Resource::LoadFromFile()�� path�� �����ߴ�.
			// �׷��� �̴� Unity�� Load�ʹ� ������ �ٸ���. �Դٰ� Load�� Resource override�� �ƴϴ�.
			// ���������� ���� �� LoadFromFile()�� �´�.
			// Texture::Load()�δ� Engine Format�� ��� Data�� �ѱ� �� ����.
		}

	private:

	private:
		std::vector<Resource*> m_Resources;	// shared_ptr��...?

		// assetŸ�Ժ��� direcoty�� �ٸ���.

		// importer��
	};
}