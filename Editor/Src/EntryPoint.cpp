
#include "Core/DiveEngine.h"
#include "Window.h"
#include "Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
#endif

	Editor editor;
	auto settingData = Dive::Settings::GetInstance();

	if (!Window::Create(hInstance, L"Dive Editor", settingData.GetResolutionWidth(), settingData.GetResolutionHeight()))
	{
		MessageBox(nullptr, L"Editor ������ ������ �����Ͽ����ϴ�.", L"Error", MB_OK);
		return 1;
	}

	// ���⿡�� ������ �����͸� ���� �Ѵ�.
	editor.Initialize();
	APP_TRACE("Editor�� �����մϴ�.");
	Window::Show();

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