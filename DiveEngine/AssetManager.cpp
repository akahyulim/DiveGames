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
	
	// Asset���� ����ȭ�ϸ鼭 ��θ� .dat�� ���� ���Ϸ� �����.
	void AssetManager::SaveAssetsToFile()
	{
		CORE_TRACE("AssetManager::SaveAssetsToFile()");
	}
	
	// .dat�� �о� Asset File���� ��� ��üȭ�Ѵ�. 
	void AssetManager::LoadAssetsFromFile()
	{
		CORE_TRACE("AssetManager::LoadAssetsFromFile()");
	}
}