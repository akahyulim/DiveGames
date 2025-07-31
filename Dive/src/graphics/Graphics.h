#pragma once

namespace Dive
{
	class Graphics
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Resize(uint32_t  width, uint32_t height);
		static void OnResizeViews();

		static void Present();

		static IDXGISwapChain* GetSwapChain();
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetRenderTargetView();
		static ID3D11DepthStencilView* GetDepthStencilView();

		static uint32_t GetWidth() { return s_width; }
		static uint32_t GetHeight() { return s_height; }

		static bool IsVSyncEnabled() { return m_useSync; }
		static void SetVSyncEnabled(bool enabled) { m_useSync = enabled; }

	private:
		static bool SetupViews();
		static bool ResizeSwapChain();

	private:
		static uint32_t s_width;
		static uint32_t s_height;
		static bool m_useSync;

		static Microsoft::WRL::ComPtr<IDXGISwapChain> s_swapChain;
		static Microsoft::WRL::ComPtr<ID3D11Device> s_device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> s_deviceContext;

		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> s_renderTargetView;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> s_depthStencilBuffer;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> s_depthStencilView;
	};
}
