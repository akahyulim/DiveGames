#include <Windows.h>
#include "Editor.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor* editor = new Editor;
	if (!editor->Initialize())
		return 0;

	while (!editor->IsExiting())
		editor->RunFrame();

	delete editor;

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}