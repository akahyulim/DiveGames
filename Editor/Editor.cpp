#include "Editor.h"

namespace Editor
{
	Editor::Editor()
	{
	}

	Editor::~Editor()
	{
	}
	
	void Editor::Initialize()
	{
		Dive::Runtime::Initialize();

		APP_TRACE("Editor의 초기화에 성공하였습니다.");
	}
}