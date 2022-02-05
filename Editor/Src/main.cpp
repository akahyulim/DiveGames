#include "Base/Editor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	// create & intialize editor
	auto pEditor = Dive::CreateApplication(hInstance);
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