
#include <DiveEngine.h>
#include "Window.h"
#include "Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();

	if (!Window::Create(hInstance, L"Dive Editor"))
	{
		MessageBox(nullptr, L"Editor ������ ������ �����Ͽ����ϴ�.", L"Error", MB_OK);
		return 1;
	}

	Dive::Log::Initialize();
	APP_TRACE("Editor�� �����մϴ�.");
	Window::Show();

	while (Window::Run())
	{

	}

	Window::Destroy();
	APP_TRACE("Editor�� �����մϴ�.");

	system("pause");
	FreeConsole();
	
	return 0;
}