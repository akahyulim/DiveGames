#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	class Graphics
	{
	public:
		Graphics();
		~Graphics() = default;

		bool Initialize(uint32_t width, uint32_t height, HWND hWnd);
		void Shutdown();

		DirectX::XMUINT2 GetResolution() { return m_Resolution; }
		void ChangeResolution(uint32_t width, uint32_t height);

		bool IsVSyncEnabled() { return m_bVSync; }
		void SetVSyncEnabled(bool enabled) { m_bVSync = enabled; }

		void OnWindowResized(EventData data);

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		ID3D11RenderTargetView* GetBackbufferView() { return m_pBackbufferView; }
		ID3D11DepthStencilView* GetBackbufferDepthView() { return m_pBackbufferDepthView; }

	private:
		bool updateBackbuffer();

	private:
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11RenderTargetView* m_pBackbufferView;
		ID3D11Texture2D* m_pBackbufferDepthTex;
		ID3D11DepthStencilView* m_pBackbufferDepthView;

		DirectX::XMUINT2 m_Resolution;
		bool m_bVSync;
	};
}