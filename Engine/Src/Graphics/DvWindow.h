#pragma once
#include <Windows.h>

namespace Dive
{
	// 윈도우 기본 가로 크기.
	const int WND_WIDTH = 800;
	// 윈도우 기본 세로 크기.
	const int WND_HEIGHT = 600;
	// 윈도우 클래스 이름.
	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	// 윈도우 모드 열거자.
	enum eDvWindowFlags : unsigned int
	{
		DV_WINDOW_FULLSCREEN = 1,
		DV_WINDOW_BORDERLESS = 1 << 1,
		DV_WINDOW_RESIZABLE = 1 << 2
	};

	class DvWindow
	{
	public:
		// 생성자. 멤버 변수 초기화.
		DvWindow();
		// 소멸자. Destroy() 호출.
		~DvWindow();

		// 윈도우 생성.
		bool Create(const std::wstring& title, int x, int y, int width, int height, unsigned int flags);
		// 윈도우 객체 제거.
		void Destroy();

		// 메시지 핸들러 실행.
		bool Run();

		// 윈도우 타이틀 변경.
		void ChangeTitle(const std::wstring& title);
		// 윈도우 타이틀 리턴.
		const std::wstring& GetTitle() const { return m_Title; }

		// 윈도우 클라이언트 영역 크기 변경.
		void Resize(int width, int height);
		// 윈도우 클라이언트 영역 크기 리턴.
		void GetSize(int& outWidth, int& outHeight) const;

		// 윈도우 위치 변경.
		void SetPosition(int x, int y);
		// 윈도우 위치 리턴.
		void GetPosition(int& outX, int& outY) const;

		// 윈도우 보이기.
		void Show();
		// 윈도우 숨기기.
		void Hide();

		// 윈도우 창 최대화. DV_WINDOW_RESIZABLE일때만 가능.
		void Maximize();
		// 윈도우 창 최소화. DV_WINDOW_RESIZABLE일때만 가능.
		void Minimize();

		// 전체화면 모드 변경.
		void SetFullscreen(bool bFullscreen);
		// 테두리없는 창 모드 변경.
		void SetBorderless(bool bBordered);

		// 인스턴스 리턴.
		HINSTANCE GetInstance() const { return m_hInstance; }
		// 윈도우 핸들 리턴.
		HWND GetHandle() const { return m_hWnd; }

		// 윈도우 스타일 리턴.
		DWORD GetStyle() const { return m_Styles; }

		// 전체 화면 모드 여부 리턴.
		bool IsFullScreen() const { return m_Flags & DV_WINDOW_FULLSCREEN; }
		// 테두리 없느 창 모드 여부 리턴.
		bool IsBorderless() const { return m_Flags & DV_WINDOW_BORDERLESS; }
		// 창 크기 변경 가능 여부 리턴.
		bool IsResizable() const { return m_Flags & DV_WINDOW_RESIZABLE; }

		/*
		* GetNumDisplayMode
		* GetDesktopDisplayMode
		* GetNumVideoDisplays(모니터 개수)
		* GetWindowDisplayIndex
		* GetDisplayDPI
		* SetWindowIcon
		* FreeSurface
		*/

	private:
		// 인스턴스.
		HINSTANCE m_hInstance;
		// 윈도우 핸들.
		HWND m_hWnd;
		// 타이틀 문구.
		std::wstring m_Title;
		// 윈도우 크기.
		DirectX::XMINT2 m_Size;
		// 윈도우 좌상 위치.
		DirectX::XMINT2 m_Position;

		// 윈도우 스타일.
		DWORD m_Styles;
		// 윈도우 Flags.
		unsigned int m_Flags;
	};
}