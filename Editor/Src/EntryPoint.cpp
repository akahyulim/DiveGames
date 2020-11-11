
#include "Core/DiveEngine.h"
#include "Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor editor;
	auto settings = Dive::Settings::GetInstance();

	// 전달: width, height, bFrame
	// 리턴: hwnd, 갱신된 width와 height
	Dive::AppWindow window(hInstance, L"Dive Editor");
	if(!window.Initialize())
	{
		APP_TRACE("초기화에 실패하여 프로그램을 종료합니다.");
		return 1;
	}

	window.Show();

	// settings member는 굳이 전달할 필요가 없다.
	// 즉, render device가 직접 확인하면 된다.
	// 지금 문제는 윈도우 핸들 자체를 멤버 변수로 가질 필요가 없기에
	// 값이나 참조로 전달하는게 맞다는 것이다.
	// 하지만 RenderDevice의 초기화는 Renderer의 Initialize 인터페이스를 벗어날 수 없다.
	if (!editor.Initialize(
		window.GetHandle(),
		settings.GetResolutionWidth(),
		settings.GetResolutionHeight(),
		settings.IsVSync(),
		settings.GetScreenMode() == Dive::eScreenMode::FullScreen ? false : true))
	{
		APP_TRACE("초기화에 실패하여 프로그램을 종료합니다.");
		return 1;
	}
	APP_TRACE("프로그램을 실행합니다.");

	while(window.Run())
	{
		editor.Update();
	}

	window.Destroy();
	APP_TRACE("프로그램을 종료합니다.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}