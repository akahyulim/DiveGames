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

		APP_TRACE("Editor�� �ʱ�ȭ�� �����Ͽ����ϴ�.");
	}
}