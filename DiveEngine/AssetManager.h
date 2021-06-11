#pragma once
#include "Resource.h"
#include <vector>

namespace Dive
{
	// Asset은 파일화된 Resource를 의미한다.
	// Asset의 Load, Save, Cache 등을 수행한다.
	// Runtime의 State를 구분하고, 이에따라 역할을 수행했으면 좋겠다.
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

		// direcoties

		// importer들
	};
}