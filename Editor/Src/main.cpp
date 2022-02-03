#include "Base/Window.h"
#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	// create window
	Dive::Window window(hInstance, Dive::WindowData(L"DiveEditor"));

	// create engine system & runtime
	Dive::Log::Initialize();
	auto pEditor = new Dive::Editor("DiveEditor");

	// tick
	while (window.Run())
	{
	}

	// close
	delete pEditor;
	pEditor = nullptr;
	window.Destroy();

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}