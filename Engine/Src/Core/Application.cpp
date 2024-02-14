#include "divepch.h"
#include "Application.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Timer.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"

namespace Dive
{
	Application* Application::s_pInstance = nullptr;
	
	HWND s_hWnd;
	HINSTANCE s_hInstance;

	constexpr LPCWSTR DV_WINCLASS_NAME = L"DIVE_WINDOW";

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	Application::Application(const std::wstring title, uint32_t windowWidth, uint32_t windowHeight, bool fullscreen)
		: m_Title(title)
		, m_WindowWidth(windowWidth)
		, m_WindowHeight(windowHeight)
		, m_bFullscreen(fullscreen)
		, m_bRunning(false)
	{
		s_pInstance = this;
	}

	Application::~Application()
	{
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		::DestroyWindow(s_hWnd);
		::UnregisterClassW(DV_WINCLASS_NAME, s_hInstance);
	}

	bool Application::InitializeWindow()
	{
		s_hInstance = GetModuleHandle(NULL);

		WNDCLASSEX wc;
		wc.style = 0;
		wc.hInstance = s_hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DV_WINCLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wc))
		{
			DV_CORE_ERROR("윈도우클래스 레지스터에 실패하였습니다.");
			return false;
		}

		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		RECT rt = { 0, 0, (LONG)m_WindowWidth, (LONG)m_WindowHeight };
		::AdjustWindowRect(&rt, style, FALSE);

		int width = (int)(rt.right - rt.left);
		int height = (int)(rt.bottom - rt.top);

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		s_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WINCLASS_NAME,
			m_Title.c_str(),
			style,
			posX, posY,
			width, height,
			NULL, NULL,
			s_hInstance,
			NULL);

		if (!s_hWnd)
		{
			DV_CORE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}

		ShowWindow(s_hWnd, SW_SHOW);
		SetFocus(s_hWnd);

		return true;
	}

	int Application::Run()
	{
		m_bRunning = true;

		OnSetup();

		if (!InitializeWindow())
		{
			DV_CORE_ERROR("윈도우 생성 및 초기화에 실패하였습니다.");
			return -1;
		}

		if(Graphics::Initialize(s_hWnd, m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			// urho를 따라갈거면 역시 engine으로 모으는 편이...?
			if (!Renderer::Initialize())
				return -1;

			if(!Input::Initialize(s_hInstance, s_hWnd))
				return -1;

			OnStart();

			MSG msg;
			ZeroMemory(&msg, sizeof(MSG));

			while (m_bRunning)
			{
				if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}

				if (msg.message == WM_QUIT)
					m_bRunning = false;
				else
				{
					Timer::Update();
					Input::Update();
					Renderer::Update();
					FIRE_EVENT(PostUpdateEvent());

					Graphics::BeginFrame();
					Renderer::Render();
					Graphics::EndFrame();
				}
			}

			OnStop();
		}
		else
		{
			DV_CORE_ERROR("엔진 초기화에 실패하여 프로그램을 종료합니다.");
			return -1;
		}

		return 0;
	}

	void Application::SetTitle(const std::wstring& title)
	{
		m_Title = title;
		SetWindowText(s_hWnd, m_Title.c_str());
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;
		Application& app = Application::GetApplication();
		
		switch (message)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}

		return result;
	}
}