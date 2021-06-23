#include "AssetManager.h"
#include "Log.h"

namespace dive
{
	AssetManager::AssetManager()
	{
		CORE_TRACE("AssetManager::AssetManager()");
	}

	AssetManager::~AssetManager()
	{
		CORE_TRACE("AssetManager::~AssetManager()");
	}
	
	// Asset들을 파일화하면서 경로를 .dat로 묶어 파일로 만든다.
	void AssetManager::SaveAssetsToFile()
	{
		CORE_TRACE("AssetManager::SaveAssetsToFile()");
	}
	
	// .dat를 읽어 Asset File들을 모두 객체화한다. 
	void AssetManager::LoadAssetsFromFile()
	{
		CORE_TRACE("AssetManager::LoadAssetsFromFile()");
	}
}