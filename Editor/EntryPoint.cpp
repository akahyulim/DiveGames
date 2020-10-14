
#include "Core/DiveEngine.h"
#include "Window.h"
#include "Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	if (!Window::Create(hInstance, L"Dive Editor"))
	{
		MessageBox(nullptr, L"Editor 윈도우 생성에 실패하였습니다.", L"Error", MB_OK);
		return 1;
	}

	Dive::Log::Initialize();
	Editor edt;
	APP_TRACE("Editor를 실행합니다.");
	Window::Show();

	while (Window::Run())
	{
		edt.Update();
	}

	Window::Destroy();
	APP_TRACE("Editor를 종료합니다.");

#ifdef _DEBUG
	system("pause");
	FreeConsole();
#endif

	return 0;
}