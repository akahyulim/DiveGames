#pragma once

/*
*	SwapChain 및 D3D11 Device 관리
*	Resource 생성 및 Bind 구현은 좀 더 생각해 봐야 할 것 같다.
*/
namespace Dive
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer = false);
		~GraphicsDevice() = default;

		void PresentBegin(float* pClearColor = nullptr);
		void PresentEnd();

		void ResizeTarget(UINT width, UINT height);
		void SetResolution(UINT width, UINT height);

		UINT GetResolutionWidth() const { return m_ResolutionWidth; }
		UINT GetResolutionHeight() const { return m_ResolutionHeight; }
		
		DXGI_FORMAT GetFormat() const { return m_Format; }
		UINT GetBackbufferCount() const { return m_BackbufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		bool IsInitialized() const;

		ID3D11Device* GetDevice() { return m_pDevice.Get(); }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext.Get(); }

		ID3D11Resource* GetBackbuffer() { return m_pBackbuffer.Get(); }
		ID3D11RenderTargetView* GetBackbufferRTV() { return m_pBackbufferRTV.Get(); }

	private:
		void createBackbufferResource();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;

		Microsoft::WRL::ComPtr<ID3D11Resource> m_pBackbuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pBackbufferRTV;

		UINT m_ResolutionWidth = 0;
		UINT m_ResolutionHeight = 0;
		bool m_bVSync = false;
		bool m_bFullScreen = false;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT m_BackbufferCount = 2;
	};
}