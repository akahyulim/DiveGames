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

// Asset 폴더에 있는 리소스들을 모두 객체화한다.
void AssetPanel::loadAssets()
{
	// 일단 단순화를 위해 임포터 기능을 엔진에 넣어두자.
	// 그리고 엔진 포멧은 만들지 말고, .dat에 외부 파일 경로를 저장하자.

	// textures
	// 텍스쳐 역시 처음부터 다 로드하지 말고
	// 파일만 보이는 상태에서 드래그하면 처음에는 로드
	// 이후에는 드래그하면 매니저에서 찾아 리턴하도록 하자.
	{
	//	Dive::Texture2D::Create("Assets/Textures/Baseplate Grid.png");
	//	Dive::Texture2D::Create("Assets/Textures/ChoA.jpg");
	//	Dive::Texture2D::Create("Assets/Textures/dmc.jpg");
	//	Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");
	//	Dive::Texture2D::Create("Assets/Textures/IU.jpg");
	}

	// 문제는 Model이다. 외부파일만으로는 부족하다. 게임 오브젝트를 포함한다.
	// 유니티와는 달리 에셋 창에서 화면으로 드래그하면 로드 및 생성을 하고
	// 이후 생성된 객체를 이용해 복사하여 사용하는 것을 생각해 볼 수 있다.
	// 하지만 이 경우 프리팹 기능이 없다.
	// 프리팹을 이용한 인스턴스화와 단순 복사는 성격이 다르다.
}
