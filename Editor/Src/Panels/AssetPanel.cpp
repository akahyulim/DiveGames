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
	}

	void AssetPanel::renderWindow()
	{
	}

	void AssetPanel::loadAssets()
	{
		// 엔진 포멧 에셋을 모두 로드하는 것이 옳을까?
		// 해당 scene의 resource.dat에 이미 로드된 것은 제외되어야 한다.

		// 그렇다면 외부 파일을 엔진 포멧으로 변경하는 시점은 언제가 맞을까?
		// 일단 최초 로드시 엔진 포멧 파일 여부를 확인하고 바로 만들어야 하는 것일까?
	}

}