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
		// 소멸자.
		~DvGraphics();

		// 초기화 완료 여부 리턴.
		bool IsInitialized() const;

		// 윈도우, D3D11 장치 및 리소스 제거.
		void Destroy();

		// 렌더링 프레임 시작.
		bool BeginFrame();
		// 렌더링 프레임 종료 후 스왑 체인.
		void EndFrame();

		// Draw 함수들: 총 5개

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		// 윈도우 타이틀 설정.
		void SetTitle(const std::wstring& title);
		// 윈도우 위치 설정.
		void SetPosition(int x, int y);
		
		
		// 윈도우, D3D11 디바이스 모드 설정 후 생성.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);

		// 임시: 윈도우 메시지 피킹 함수 호출.
		bool RunWindow();

		// 임시
		HWND GetWindowHandle();

		// 임시
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }

	private:
		// D3D11 Device, SwapChain 생성.
		bool createDevice(int width, int height);
		// 후면 버퍼 크기 변경에 맞춰 기본 텍스쳐 재생성.
		bool updateSwapChain(int width, int height);

	private:
		// 윈도우 객체 포인터.
		std::unique_ptr<DvWindow> m_pWindow;
		// 윈도우 타이틀. => 제거 대상.
		std::wstring m_WindowTitle;
		// 윈도우 중심 위치. => 제거 대상.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// 스왑체인 생성시 저장된다.
		int m_Width;
		int m_Height;
		// 디폴트 모드 같은데, 초기화 및 설정하는 부분을 찾지 못했다.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// Device
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		// 후면 버퍼 렌더타겟뷰.
		ID3D11RenderTargetView* m_pDefaultRenderTargetView = nullptr;
		// 기본 깊이 스텐실 텍스쳐.
		ID3D11Texture2D* m_pDefaultDepthStencilTexture = nullptr;
		// 기본 깊이 스텐실 뷰.
		ID3D11DepthStencilView* m_pDefaultDepthStencilView = nullptr;
	};
}