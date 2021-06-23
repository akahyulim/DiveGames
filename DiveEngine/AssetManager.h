#pragma once
#include "Resource.h"
#include <vector>

namespace dive
{
	// 1. unity�� package��� ���� ���Ϸ� asset�� �����Ѵ�.
	// asset Ÿ�Ժ� ������ �����ϰ����� ������ �� �𸣰ڴ�.
	// �׳� ���� ������ ���ϵ��� �����ϴ� �� ���⵵ �ϴ�.
	// 2. import�ϸ� �ش� package�� ��� asset���� prefab���� �����Ǵ� �� ����.
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