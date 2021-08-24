#pragma once
#include <d3d11_3.h>
#include <DXGI1_3.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace dive
{
	struct GraphicsData
	{
		// frame rate도 추가??
		// 그런데 이렇게 구조체로 만들어 놓았는데
		// 정작 GraphicsDevice에선 개별 변수로 관리하는 것도 이상하다.
		// 스파르탄처럼 윈도우 데이터만 받고 나머지는 Renderer에서 설정하는게 맞는 걸까나...
		GraphicsData(HWND hwnd, bool fullScreen, bool vsync, DXGI_FORMAT format, UINT bufferCount)
		{
			this->hwnd = hwnd;
			this->fullScreen = fullScreen;
			this->vsync = vsync;
			this->format = format;
			this->bufferCount = bufferCount;
		}

		HWND hwnd = 0;
		bool fullScreen = false;
		bool vsync = false;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT bufferCount = 2;
	};

	class dvGraphicsDevice
	{
	public:
		dvGraphicsDevice(HWND hWnd, bool fullScreen = false, bool vsync = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT bufferCount = 2, bool debugLayer = false);
		dvGraphicsDevice(const GraphicsData& data, bool debugLayer = false);
		~dvGraphicsDevice();

		bool IsInitialized();

		void ResizeResolution(UINT width, UINT height);
		void ResizeBackbuffer(UINT width, UINT height);

		ID3D11Resource* GetBackbuffer() { return m_pBackbuffer; }
		ID3D11RenderTargetView* GetBackbufferRTV() { return m_pBackbufferRTV; }

		bool IsFullScreen() const { return m_bFullScreen; }

		bool VSyncEnabled() const { return m_bVSync; }
		void SetVSync(bool vsync) { m_bVSync = vsync; }

		DXGI_FORMAT GetFormat() const { return m_Format; }

		UINT GetResolutionWidth() const { return m_ResolutionWidth; }
		UINT GetResolutionHeight() const { return m_ResolutionHeight; }

	private:
		bool createBackbufferResource();
		void clearBackbufferResource();

	private:
		IDXGISwapChain* m_pSwapChain = nullptr;
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;

		ID3D11Resource* m_pBackbuffer = nullptr;
		ID3D11RenderTargetView* m_pBackbufferRTV = nullptr;

		bool m_bFullScreen = false;
		bool m_bVSync = false;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		UINT m_BufferCount = 0;

		UINT m_ResolutionWidth = 0;
		UINT m_ResolutionHeight = 0;
	};
}
