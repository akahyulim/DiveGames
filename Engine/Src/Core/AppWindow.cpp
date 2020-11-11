#include "DivePch.h"
#include "AppWindow.h"
#include "Log.h"
#include "Settings.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	AppWindow::AppWindow(HINSTANCE hInstance, std::wstring name)
		: m_hInstance(hInstance), m_name(std::move(name))
	{
	}

	AppWindow::~AppWindow()
	{
		if (m_hWnd || m_hInstance)
			Destroy();
	}

	bool AppWindow::Initialize()
	{
		// wnd class
		WNDCLASSEX wndClass;
		wndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // 0;
		wndClass.lpfnWndProc	= WndProc;
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.hInstance		= m_hInstance;
		wndClass.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hIconSm		= wndClass.hIcon;
		wndClass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		wndClass.lpszMenuName	= nullptr;
		wndClass.lpszClassName	= m_name.c_str();
		wndClass.cbSize			= sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wndClass))
		{
			MessageBox(nullptr, L"윈도우 클래스 등록에 실패하였습니다.", L"Error", MB_OK);
			CORE_ERROR("윈도우 클래스 등록에 실패하였습니다.");
			return false;
		}

		// size and style

		// create window
		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,//WS_EX_CLIENTEDGE,
			m_name.c_str(),
			m_name.c_str(),
			WS_OVERLAPPEDWINDOW,
			0, 0,
			800, 600,
			nullptr, nullptr, m_hInstance, nullptr);

		if (!m_hWnd)
		{
			MessageBox(nullptr, L"윈도우 생성에 실패하였습니다.", L"Error", MB_OK);
			CORE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	void AppWindow::Destroy()
	{
		DestroyWindow(m_hWnd);
		UnregisterClass(L"WndClass", m_hInstance);

		m_hWnd = NULL; m_hInstance = NULL;
	}

	bool AppWindow::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	void AppWindow::Show()
	{
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	bool AppWindow::calcuSize()
	{
		auto settings = Settings::GetInstance();
		auto mode = settings.GetScreenMode();
		auto width = settings.GetResolutionWidth();
		auto height = settings.GetResolutionHeight();

		// width, height, screen mode가 필요하다.

		// 일단 현재 디바이스가 지원하는 크기인지 확인 필요하다.
		// 최초 실행시 어딘가에 저장해 놓아야 한다.

		// windowed
		// frame을 제외한 크기를 확보해야 한다.
		// 최대 해상도 윈도우일 경우 ui가 어떻게 되는지 확인이 필요하다.

		// borderless
		// 디바이스가 지원하는 최대크기 이하여야 한다.

		// fullscreen
		// 왠만하면 해상도는 건드리지 말아야 한다.
		// 즉, 지원 최대해상도로만 만들어야 한다.
		// 실제 게임에서는 어떠한지 확인이 필요하다.

		return false;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		if ((msg == WM_SIZE) || (msg == WM_DISPLAYCHANGE))
		{
			auto width = static_cast<int>(lParam & 0xffff);
			auto height = static_cast<int>((lParam >> 16) & 0xffff);

			WindowResizeEvent evnt(width, height);
			EventSystem::GetInstance().Fire(&evnt);
		}
		else if (msg == WM_CLOSE)
		{
			PostQuitMessage(0);
		}
		else
		{
			result = DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return result;
	}
}