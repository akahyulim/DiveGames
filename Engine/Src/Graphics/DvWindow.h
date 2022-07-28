#pragma once
#include <Windows.h>

namespace Dive
{
	// ������ �⺻ ���� ũ��.
	const int WND_WIDTH = 800;
	// ������ �⺻ ���� ũ��.
	const int WND_HEIGHT = 600;
	// ������ Ŭ���� �̸�.
	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	// ������ ��� ������.
	enum eDvWindowFlags : unsigned int
	{
		DV_WINDOW_FULLSCREEN = 1,
		DV_WINDOW_BORDERLESS = 1 << 1,
		DV_WINDOW_RESIZABLE = 1 << 2
	};

	class DvWindow
	{
	public:
		// ������. ��� ���� �ʱ�ȭ.
		DvWindow();
		// �Ҹ���. Destroy() ȣ��.
		~DvWindow();

		// ������ ����.
		bool Create(const std::wstring& title, int x, int y, int width, int height, unsigned int flags);
		// ������ ��ü ����.
		void Destroy();

		// �޽��� �ڵ鷯 ����.
		bool Run();

		// ������ Ÿ��Ʋ ����.
		void ChangeTitle(const std::wstring& title);
		// ������ Ÿ��Ʋ ����.
		const std::wstring& GetTitle() const { return m_Title; }

		// ������ Ŭ���̾�Ʈ ���� ũ�� ����.
		void Resize(int width, int height);
		// ������ Ŭ���̾�Ʈ ���� ũ�� ����.
		void GetSize(int& outWidth, int& outHeight) const;

		// ������ ��ġ ����.
		void SetPosition(int x, int y);
		// ������ ��ġ ����.
		void GetPosition(int& outX, int& outY) const;

		// ������ ���̱�.
		void Show();
		// ������ �����.
		void Hide();

		// ������ â �ִ�ȭ. DV_WINDOW_RESIZABLE�϶��� ����.
		void Maximize();
		// ������ â �ּ�ȭ. DV_WINDOW_RESIZABLE�϶��� ����.
		void Minimize();

		// ��üȭ�� ��� ����.
		void SetFullscreen(bool bFullscreen);
		// �׵θ����� â ��� ����.
		void SetBorderless(bool bBordered);

		// �ν��Ͻ� ����.
		HINSTANCE GetInstance() const { return m_hInstance; }
		// ������ �ڵ� ����.
		HWND GetHandle() const { return m_hWnd; }

		// ������ ��Ÿ�� ����.
		DWORD GetStyle() const { return m_Styles; }

		// ��ü ȭ�� ��� ���� ����.
		bool IsFullScreen() const { return m_Flags & DV_WINDOW_FULLSCREEN; }
		// �׵θ� ���� â ��� ���� ����.
		bool IsBorderless() const { return m_Flags & DV_WINDOW_BORDERLESS; }
		// â ũ�� ���� ���� ���� ����.
		bool IsResizable() const { return m_Flags & DV_WINDOW_RESIZABLE; }

		/*
		* GetNumDisplayMode
		* GetDesktopDisplayMode
		* GetNumVideoDisplays(����� ����)
		* GetWindowDisplayIndex
		* GetDisplayDPI
		* SetWindowIcon
		* FreeSurface
		*/

	private:
		// �ν��Ͻ�.
		HINSTANCE m_hInstance;
		// ������ �ڵ�.
		HWND m_hWnd;
		// Ÿ��Ʋ ����.
		std::wstring m_Title;
		// ������ ũ��.
		DirectX::XMINT2 m_Size;
		// ������ �»� ��ġ.
		DirectX::XMINT2 m_Position;

		// ������ ��Ÿ��.
		DWORD m_Styles;
		// ������ Flags.
		unsigned int m_Flags;
	};
}