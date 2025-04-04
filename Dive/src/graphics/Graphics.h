#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	class Graphics
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void ChangeResolution(uint32_t width, uint32_t height);

		static void OnWindowResized(EventData data);

		static void Present();

		static IDXGISwapChain* GetSwapChain();
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetBackbufferRTV();
		static ID3D11DepthStencilView* GetBackbufferDSV();

		static uint32_t GetResolutionWidth() { return s_ResolutionWidth; }
		static uint32_t GetResolutionHeight() { return s_ResolutionHeight; }

		static bool IsVSyncEnabled() { return s_UseVSync; }
		static void SetVSyncEnabled(bool enabled) { s_UseVSync = enabled; }

	private:
		static void updateBackbuffer();

	private:
		static IDXGISwapChain* s_SwapChain;
		static ID3D11Device* s_Device;
		static ID3D11DeviceContext* s_DeviceContext;

		static ID3D11RenderTargetView* s_RenderTargetView;
		static ID3D11Texture2D* s_DepthStencilTexture;
		static ID3D11DepthStencilView* s_DEpthStencilView;

		static uint32_t s_ResolutionWidth;
		static uint32_t s_ResolutionHeight;
		static bool s_UseVSync;
	};
}
