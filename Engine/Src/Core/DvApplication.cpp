#include "divepch.h"
#include "DvApplication.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Timer.h"
#include "Graphics/GraphicsDevice.h"
#include "Renderer/DvRenderer.h"
#include "Resource/ResourceCache.h"
#include "Input/DvInput.h"

namespace Dive
{
	DvApplication* DvApplication::s_pInstance = nullptr;
	
	HWND s_hWnd;
	HINSTANCE s_hInstance;

	constexpr LPCWSTR DV_WINCLASS_NAME = L"DIVE_WINDOW";

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	DvApplication::DvApplication(const std::wstring title, uint32_t windowWidth, uint32_t windowHeight, bool fullscreen)
		: m_Title(title)
		, m_WindowWidth(windowWidth)
		, m_WindowHeight(windowHeight)
		, m_bFullscreen(fullscreen)
		, m_bRunning(false)
	{
		s_pInstance = this;
	}

	DvApplication::~DvApplication()
	{
		ResourceCache::Shutdown();
		DvRenderer::Shutdown();
		GraphicsDevice::Shutdown();

		::DestroyWindow(s_hWnd);
		::UnregisterClassW(DV_WINCLASS_NAME, s_hInstance);
	}

	bool DvApplication::InitializeWindow()
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
			DV_CORE_ERROR("������Ŭ���� �������Ϳ� �����Ͽ����ϴ�.");
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
			DV_CORE_ERROR("������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		ShowWindow(s_hWnd, SW_SHOW);
		SetFocus(s_hWnd);

		return true;
	}

	int DvApplication::Run()
	{
		m_bRunning = true;

		OnSetup();

		if (!InitializeWindow())
		{
			DV_CORE_ERROR("������ ���� �� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return -1;
		}

		if(GraphicsDevice::Initialize(s_hWnd, m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			// urho�� ���󰥰Ÿ� ���� engine���� ������ ����...?
			if (!DvRenderer::Initialize())
				return -1;

			if(!DvInput::Initialize(s_hInstance, s_hWnd))
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
					DvInput::Update();
					DvRenderer::Update();
					FIRE_EVENT(PostUpdateEvent());

					GraphicsDevice::BeginFrame();
					DvRenderer::Render();
					GraphicsDevice::EndFrame();
				}
			}

			OnStop();
		}
		else
		{
			DV_CORE_ERROR("���� �ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
			return -1;
		}

		return 0;
	}

	void DvApplication::SetTitle(const std::wstring& title)
	{
		m_Title = title;
		SetWindowText(s_hWnd, m_Title.c_str());
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;
		DvApplication& app = DvApplication::GetApplication();
		
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