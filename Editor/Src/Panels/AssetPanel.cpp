#include "AssetPanel.h"

AssetPanel::AssetPanel(Editor* pEditor)
	: Panel(pEditor, "Asset")
{
	// temp
	loadAssets();
}

AssetPanel::~AssetPanel()
{
}

void AssetPanel::renderWindow()
{
}

// Asset ������ �ִ� ���ҽ����� ��� ��üȭ�Ѵ�.
void AssetPanel::loadAssets()
{
	// �ϴ� �ܼ�ȭ�� ���� ������ ����� ������ �־����.
	// �׸��� ���� ������ ������ ����, .dat�� �ܺ� ���� ��θ� ��������.

	// textures
	// �ؽ��� ���� ó������ �� �ε����� ����
	// ���ϸ� ���̴� ���¿��� �巡���ϸ� ó������ �ε�
	// ���Ŀ��� �巡���ϸ� �Ŵ������� ã�� �����ϵ��� ����.
	{
	//	Dive::Texture2D::Create("Assets/Textures/Baseplate Grid.png");
	//	Dive::Texture2D::Create("Assets/Textures/ChoA.jpg");
	//	Dive::Texture2D::Create("Assets/Textures/dmc.jpg");
	//	Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");
	//	Dive::Texture2D::Create("Assets/Textures/IU.jpg");
	}

	// ������ Model�̴�. �ܺ����ϸ����δ� �����ϴ�. ���� ������Ʈ�� �����Ѵ�.
	// ����Ƽ�ʹ� �޸� ���� â���� ȭ������ �巡���ϸ� �ε� �� ������ �ϰ�
	// ���� ������ ��ü�� �̿��� �����Ͽ� ����ϴ� ���� ������ �� �� �ִ�.
	// ������ �� ��� ������ ����� ����.
	// �������� �̿��� �ν��Ͻ�ȭ�� �ܼ� ����� ������ �ٸ���.
}
