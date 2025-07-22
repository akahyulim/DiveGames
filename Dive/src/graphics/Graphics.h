#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	class Shader;
	class RenderTexture;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;

	class Graphics
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void ChangeResolution(uint32_t width, uint32_t height);

		static void OnWindowResized(EventData data);

		static void Present();

		static IDXGISwapChain* GetSwapChain();
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetBackBufferRTV();
		static ID3D11DepthStencilView* GetBackBufferDSV();

		static uint32_t GetResolutionWidth() { return s_ResolutionWidth; }
		static uint32_t GetResolutionHeight() { return s_ResolutionHeight; }

		static bool IsVSyncEnabled() { return s_UseVSync; }
		static void SetVSyncEnabled(bool enabled) { s_UseVSync = enabled; }

	private:
		static void updateBackbuffer();

	private:
		static uint32_t s_ResolutionWidth;
		static uint32_t s_ResolutionHeight;
		static bool s_UseVSync;

		static Microsoft::WRL::ComPtr<IDXGISwapChain> s_SwapChain;
		static Microsoft::WRL::ComPtr<ID3D11Device> s_Device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> s_DeviceContext;

		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> s_BackBufferRTV;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> s_BackbufferTexture;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> s_BackBufferDSV;
	};
}
