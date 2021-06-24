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