#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	// create editor
	Dive::Editor* pEditor = new Dive::Editor(hInstance);
	DV_ASSERT(pEditor);

	// run editor
	pEditor->Run();

	// delete editor
	DV_DELETE(pEditor);

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}