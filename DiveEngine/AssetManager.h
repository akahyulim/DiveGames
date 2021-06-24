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
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();



		void SaveAssetsToFile();
		void LoadAssetsFromFile();

	private:

	private:
		std::vector<Resource*> m_assets;	// shared_ptr��...?

		// assetŸ�Ժ��� direcoty�� �ٸ���.

		// importer��
	};
}