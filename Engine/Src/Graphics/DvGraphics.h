#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;
	class DvWindow;

	struct ScreenModeParams
	{
		bool operator==(const ScreenModeParams& rhs) const
		{
			return bFullScreen == rhs.bFullScreen &&
				bBorderless == rhs.bBorderless &&
				bResizable == rhs.bResizable &&
				bVSync == rhs.bVSync;
		}

		bool operator!=(const ScreenModeParams& rhs) const
		{
			return !(*this == rhs);
		}

		bool bFullScreen = false;
		bool bBorderless = false;
		bool bResizable = false;
		bool bVSync = false;
		//bool bTripleBuffer;
		//int multiSample
		//int refreshRate;
	};

	struct WindowModeParams
	{
		int width = 0;
		int height = 0;
		ScreenModeParams screenModeParams;
	};

	class DvGraphics : public DvObject
	{ 
		DIVE_OBJECT(DvGraphics, DvObject)

	public:
		// 생성자.
		explicit DvGraphics(DvContext* pContext);
		// 소멸자
		~DvGraphics();

		// 초기화 여부 리턴.
		bool IsInitialized() const;

		// 렌더링 프레임 시작.
		bool BeginFrame();
		// 렌더링 프레임 종료 후 스왑 체인.
		void EndFrame();

		// 윈도우 타이틀 설정.
		void SetWindowTitle(const std::wstring& title);
		// 윈도우 중심 위치 설정.
		void SetWindowPosition(int x, int y);
		
		
		// 윈도우, D3D11 디바이스 모드 설정 후 생성.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullScreen, bool bBorderless, bool bResizable, bool bVSync);

		// 임시: 윈도우 메시지 피킹 함수 호출.
		bool RunWindow();

	private:
	private:
		// 윈도우 객체 포인터.
		std::unique_ptr<DvWindow> m_pWindow;
		// 윈도우 타이틀.
		std::wstring m_WindowTitle;
		// 윈도우 중심 위치.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// 스왑체인 생성시 저장된다.
		int m_Width;
		int m_Height;
		// 디폴트 모드 같은데, 초기화 및 설정하는 부분을 찾지 못했다.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// Device
	};
}