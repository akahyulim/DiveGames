#include "AssetPanel.h"

namespace Editor
{
	AssetPanel::AssetPanel(Editor* pEditor)
		: Panel(pEditor, "Asset")
	{
		// temp
		loadAssets();
	}

	AssetPanel::~AssetPanel()
	{
		DV_LOG_CLIENT_TRACE("AssetPanel �Ҹ��� ȣ��");
	}

	void AssetPanel::renderWindow()
	{
	}

	void AssetPanel::loadAssets()
	{
		// ���� ���� ������ ��� �ε��ϴ� ���� ������?
		// �ش� scene�� resource.dat�� �̹� �ε�� ���� ���ܵǾ�� �Ѵ�.

		// �׷��ٸ� �ܺ� ������ ���� �������� �����ϴ� ������ ������ ������?
		// �ϴ� ���� �ε�� ���� ���� ���� ���θ� Ȯ���ϰ� �ٷ� ������ �ϴ� ���ϱ�?
	}

}