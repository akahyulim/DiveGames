#include "Base/AppWindow.h"
#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	// create
	Dive::Editor editor;
	Dive::AppWindow window(hInstance, "DiveEditor");

	// run
	while (window.Run())
	{
		editor.Tick();
	}

	// destroy
	window.Destroy();

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}