#pragma once
#include "Resource.h"
#include <vector>

namespace Dive
{
	// Asset�� ����ȭ�� Resource�� �ǹ��Ѵ�.
	// Asset�� Load, Save, Cache ���� �����Ѵ�.
	// Runtime�� State�� �����ϰ�, �̿����� ������ ���������� ���ڴ�.
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

		// direcoties

		// importer��
	};
}