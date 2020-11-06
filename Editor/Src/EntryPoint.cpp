
#include "Core/DiveEngine.h"
#include "Window.h"
#include "Editor.h"

extern HINSTANCE Window::g_hInstance;
extern HWND Window::g_hWnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor editor;
	if (!Window::Create(hInstance, L"Dive Editor"))
		return 1;

	Window::Show();

	editor.Initialize(Window::g_hInstance, Window::g_hWnd);
	APP_TRACE("Editor�� �����մϴ�.");

	while (Window::Run())
	{
		editor.Update();
	}

	Window::Destroy();
	APP_TRACE("Editor�� �����մϴ�.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}