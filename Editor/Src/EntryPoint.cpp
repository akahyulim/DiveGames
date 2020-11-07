
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
	auto settings = Dive::Settings::GetInstance();

	// settings를 싱글톤으로 두는게 맞는가?
	// Settings 객체를 매개변수로 전달한다면 결합도는 동일하게 높아진다.
	// 하지만 윈도우 내부에서 클라이언트 영역 계산을 수행한 후 갱신이 필요하다.

	if (!Window::Create(hInstance, L"Dive Editor"))
	{
		APP_TRACE("초기화에 실패하여 프로그램을 종료합니다.");
		return 1;
	}

	Window::Show();

	if (!editor.Initialize(
		Window::g_hWnd,
		settings.GetResolutionWidth(),
		settings.GetResolutionHeight(),
		settings.IsVSync(),
		settings.GetScreenMode() == Dive::eScreenMode::FullScreen ? false : true))
	{
		APP_TRACE("초기화에 실패하여 프로그램을 종료합니다.");
		return 1;
	}
	APP_TRACE("프로그램을 실행합니다.");

	while (Window::Run())
	{
		editor.Update();
	}

	Window::Destroy();
	APP_TRACE("프로그램을 종료합니다.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}