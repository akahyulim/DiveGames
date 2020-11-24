#include "Core/DiveEngine.h"
#include "Window.h"
#include "Editor.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Window::Create(hInstance, L"Dive Editor");
	Window::Show();

	Editor editor(hInstance, Window::g_hWnd, Window::GetWidth(), Window::GetHeight(), true);
	if (!editor.IsInitialized())
	{
		APP_CRITICAL("");
		return 1;
	}

	Window::g_OnResize = [&editor](int width, int height) {editor.OnResize(width, height); };

	APP_TRACE("프로그램을 실행합니다.");
	while(Window::Run())
	{
		editor.Update();
	}

	Window::Destroy();
	APP_TRACE("프로그램이 종료되었습니다.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}