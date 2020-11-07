
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

	// settings�� �̱������� �δ°� �´°�?
	// Settings ��ü�� �Ű������� �����Ѵٸ� ���յ��� �����ϰ� ��������.
	// ������ ������ ���ο��� Ŭ���̾�Ʈ ���� ����� ������ �� ������ �ʿ��ϴ�.

	if (!Window::Create(hInstance, L"Dive Editor"))
	{
		APP_TRACE("�ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
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
		APP_TRACE("�ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
		return 1;
	}
	APP_TRACE("���α׷��� �����մϴ�.");

	while (Window::Run())
	{
		editor.Update();
	}

	Window::Destroy();
	APP_TRACE("���α׷��� �����մϴ�.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}