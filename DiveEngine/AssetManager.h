#pragma once
#include "Resource.h"
#include <vector>

namespace dive
{
	// 1. unity는 package라는 포멧 파일로 asset을 관리한다.
	// asset 타입별 구분이 존재하겠지만 솔직히 잘 모르겠다.
	// 그냥 여러 포멧의 파일들을 압축하는 것 같기도 하다.
	// 2. import하면 해당 package의 모든 asset들이 prefab으로 생성되는 것 같다.
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();



		void SaveAssetsToFile();
		void LoadAssetsFromFile();

	private:

	private:
		std::vector<Resource*> m_assets;	// shared_ptr로...?

		// asset타입별로 direcoty가 다르다.

		// importer들
	};
}