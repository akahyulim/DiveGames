#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	// create & initialize
	Dive::Editor editor(hInstance);
	editor.Initialize();

	// run
	editor.Tick();

	// destroy
	editor.Destroy();

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}