#pragma once
#include "GraphicsDefs.h"

namespace Dive
{
	class Graphics
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static bool WindowCreate();

		static bool RunWindow();

		static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static bool IsInitialized();

		static HINSTANCE GetWindowInstance();
		static HWND GetWindowHandle();
		static std::wstring GetWindowTitle();
		static int GetWidth();
		static int GetHeight();

		static void SetWindowTitle(const std::wstring& title);
		
		static bool CreateDeviceAndSwapChain();

		static bool UpdateSwapChain(uint32_t width = 0, uint32_t height = 0);

		static bool BeginFrame();
		static void EndFrame();

		static void ClearRenderTargets(uint32_t target, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil);

		static void SetRenderTargetViews(uint32_t index, ID3D11RenderTargetView* pViews);
		static void SetDepthStencilView(ID3D11DepthStencilView* pView);

		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetDefaultRenderTargetView();
		static ID3D11DepthStencilView* GetDefaultDepthStencilView();

	private:
		static void prepareDraw();
	};
}
