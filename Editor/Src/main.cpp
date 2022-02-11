#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor editor(hInstance);

	editor.Run();

	editor.Shutdown();

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}