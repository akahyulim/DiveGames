
#include "Core/DiveEngine.h"
#include "Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor editor;
	auto settings = Dive::Settings::GetInstance();

	// ����: width, height, bFrame
	// ����: hwnd, ���ŵ� width�� height
	Dive::AppWindow window(hInstance, L"Dive Editor");
	if(!window.Initialize())
	{
		APP_TRACE("�ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
		return 1;
	}

	window.Show();

	// settings member�� ���� ������ �ʿ䰡 ����.
	// ��, render device�� ���� Ȯ���ϸ� �ȴ�.
	// ���� ������ ������ �ڵ� ��ü�� ��� ������ ���� �ʿ䰡 ���⿡
	// ���̳� ������ �����ϴ°� �´ٴ� ���̴�.
	// ������ RenderDevice�� �ʱ�ȭ�� Renderer�� Initialize �������̽��� ��� �� ����.
	if (!editor.Initialize(
		window.GetHandle(),
		settings.GetResolutionWidth(),
		settings.GetResolutionHeight(),
		settings.IsVSync(),
		settings.GetScreenMode() == Dive::eScreenMode::FullScreen ? false : true))
	{
		APP_TRACE("�ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
		return 1;
	}
	APP_TRACE("���α׷��� �����մϴ�.");

	while(window.Run())
	{
		editor.Update();
	}

	window.Destroy();
	APP_TRACE("���α׷��� �����մϴ�.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}