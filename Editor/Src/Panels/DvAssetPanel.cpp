#include "DvAssetPanel.h"

namespace Editor
{
	DvAssetPanel::DvAssetPanel(DvEditor* pEditor)
		: DvPanel(pEditor, "Asset")
	{
		// temp
		loadAssets();
	}

	DvAssetPanel::~DvAssetPanel()
	{
	}

	void DvAssetPanel::renderWindow()
	{
	}

	void DvAssetPanel::loadAssets()
	{
		// ���� ���� ������ ��� �ε��ϴ� ���� ������?
		// �ش� scene�� resource.dat�� �̹� �ε�� ���� ���ܵǾ�� �Ѵ�.

		// �׷��ٸ� �ܺ� ������ ���� �������� �����ϴ� ������ ������ ������?
		// �ϴ� ���� �ε�� ���� ���� ���� ���θ� Ȯ���ϰ� �ٷ� ������ �ϴ� ���ϱ�?
	}

}