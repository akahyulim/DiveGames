#include "stdafx.h"
#include "Window.h"
#include "Log.h"

namespace Dive
{
	constexpr LPCWSTR DV_WND_CLASS_NAME = L"DiveWindow";
	constexpr uint32_t DV_WND_WIDTH = 800;
	constexpr uint32_t DV_WND_HEIGHT = 600;

	Window::Window()
		: m_hInstance(nullptr)
		, m_hWnd(nullptr)
		, m_Title(L"Dive")
		, m_bWindowed(true)
	{
	}

	Window::Window(const WindowData& data)
		: m_hInstance(::GetModuleHandle(nullptr))
		, m_hWnd(nullptr)
		, m_Title(data.Title)
		, m_bWindowed(true)
		, m_Width(data.Width)
		, m_Height(data.Height)
	{
		if (!initialize()) {
			throw std::runtime_error("������ ���� �� �ʱ�ȭ ����");
		}
	}

	bool Window::Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle)
	{
		m_hInstance = hInstance;
		m_Title = pTitle;

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = ::DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DV_WND_CLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc))
		{
			DV_LOG(Window, err, "������ Ŭ���� ��� ����");
			return false;
		}

		DWORD style = WS_OVERLAPPEDWINDOW;
		m_bWindowed = true;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(width)) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(height)) / 2;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WND_CLASS_NAME,
			m_Title.c_str(),
			style,
			posX > 0 ? posX : 0,
			posY > 0 ? posY : 0,
			DV_WND_WIDTH, DV_WND_HEIGHT,			// ���ʿ� ����Ʈ ũ��� ����
			NULL, NULL,
			m_hInstance,
			NULL);

		if (!m_hWnd)
		{
			DV_LOG(Window, critical, "������ ���� ����");
			return false;
		}

		DV_LOG(Window, trace, "�ʱ�ȭ ����");

		return Resize(width, height);
	}

	void Window::Shutdown()
	{
		::ShowCursor(TRUE);

		::DestroyWindow(m_hWnd);
		m_hWnd = nullptr;

		::UnregisterClass(DV_WND_CLASS_NAME, m_hInstance);
		m_hInstance = nullptr;

		DV_LOG(Window, trace, "�˴ٿ� ����");
	}

	bool Window::Run()
	{
		MSG msg{};
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	// width, height�� ���� 0���� ������ ��� â or �������� ��常 ���� 
	bool Window::Resize(uint32_t width, uint32_t height, bool windowed)
	{
		if (m_hWnd)
		{
			DWORD style = windowed ? WS_OVERLAPPEDWINDOW : WS_POPUP;

			if (m_bWindowed != windowed)
			{
				m_bWindowed = windowed;

				if (!::SetWindowLongPtr(m_hWnd, GWL_STYLE, style))
				{
					DV_LOG(Window, err, "������ ��Ÿ�� ���� ����");
					return false;
				}
			}

			RECT rt = { 0, 0,
				width != 0 ? static_cast<LONG>(width) : static_cast<LONG>(GetWidth()),
				height != 0 ? static_cast<LONG>(height) : static_cast<LONG>(GetHeight()) };

			if (!::AdjustWindowRect(&rt, style, FALSE))
			{
				DV_LOG(Window, err, "��Ÿ�Ͽ� ���� ������ ũ�� ��� ����");
				return false;
			}

			int newWidth = rt.right - rt.left;
			int newHeight = rt.bottom - rt.top;
			int posX = (::GetSystemMetrics(SM_CXSCREEN) - newWidth) / 2;
			int posY = (::GetSystemMetrics(SM_CYSCREEN) - newHeight) / 2;

			if (!::SetWindowPos(m_hWnd, NULL, posX, posY, newWidth, newHeight, SWP_DRAWFRAME))
			{
				DV_LOG(Window, err, "������ ũ�� ���� ����");
				return false;
			}

			ShowWindow(SW_SHOW);
		}

		return true;
	}

	DirectX::XMUINT2 Window::GetSize()
	{
		if (!m_hWnd)
			return { 0, 0 };

		RECT rt{};
		::GetClientRect(m_hWnd, &rt);

		return { static_cast<uint32_t>(rt.right - rt.left), static_cast<uint32_t>(rt.bottom - rt.top) };
	}

	uint32_t Window::GetWidth()
	{
		return GetSize().x;
	}

	uint32_t Window::GetHeight()
	{
		return GetSize().y;
	}

	void Window::SetTitle(LPCWSTR title)
	{
		if (title != nullptr)
		{
			m_Title = title;

			if (m_hWnd)
			{
				::SetWindowText(m_hWnd, title);
			}
		}
	}

	void Window::ShowWindow(int nCmdShow)
	{
		::ShowWindow(m_hWnd, nCmdShow);
	}

	bool Window::ChangeProc(LONG_PTR pWndProc)
	{
		if (!SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, pWndProc))
		{
			DV_LOG(Window, err, "���ν��� �Լ� ���濡 �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	std::unique_ptr<Window> Window::Create(const WindowData& data)
	{
		try {
			return std::make_unique<Window>(data);
		}
		catch (const std::exception&) {
			return nullptr;
		}
	}

	bool Window::initialize()
	{
		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = ::DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DV_WND_CLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc))
		{
			DV_LOG(Window, err, "������ Ŭ���� ��� ����");
			return false;
		}

		DWORD style = WS_OVERLAPPEDWINDOW;
		m_bWindowed = true;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(m_Width)) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(m_Height)) / 2;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WND_CLASS_NAME,
			m_Title.c_str(),
			style,
			posX > 0 ? posX : 0,
			posY > 0 ? posY : 0,
			DV_WND_WIDTH, DV_WND_HEIGHT,			// ���ʿ� ����Ʈ ũ��� ����
			NULL, NULL,
			m_hInstance,
			NULL);

		if (!m_hWnd)
		{
			DV_LOG(Window, critical, "������ ���� ����");
			return false;
		}

		DV_LOG(Window, trace, "�ʱ�ȭ ����");

		return Resize(m_Width, m_Height);
	}
}