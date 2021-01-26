#pragma once
#include "Core/Object.h"

namespace Dive
{
	class DeviceAndSwapChain : public Object
	{
		DIVE_OBJECT(DeviceAndSwapChain, Object);

	public:
		DeviceAndSwapChain(
			Context* context,
			HWND hWnd, 
			unsigned int width, 
			unsigned int height, 
			bool windowed						 = true, 
			unsigned int bufferCount			 = 1, 
			DXGI_FORMAT format					 = DXGI_FORMAT_R8G8B8A8_UNORM, 
			bool vSync							 = true,
			unsigned int refreshRateNumerator	 = 60,
			unsigned int refreshRateDenominator	 = 1);
		~DeviceAndSwapChain();

		void Present();

		bool IsInitialized() const { return m_bInitialized; }

		bool ResizeBuffer(DirectX::XMUINT2 size = DirectX::XMUINT2(0, 0));
		bool ResizeTarget(DirectX::XMUINT2 size, bool force = false);
		DirectX::XMUINT2 GetSwapChainSize() const { return m_size; }
		void SetFullScreen(bool fullScreen);

		ID3D11Device* GetDevice() const { return m_device; }
		ID3D11DeviceContext* GetImmediateContext() const { return m_immediateContext; }
		IDXGISwapChain* GetSwapChain() const { return m_swapChain; }

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView; }

		DirectX::XMFLOAT4 GetClearColor() const { return m_clearColor; }
		void SetClearColor(DirectX::XMFLOAT4 color) { m_clearColor = color; }

		DirectX::XMUINT2 GetRefreshRate() const { return m_refreshRate; }

	private:
		DeviceAndSwapChain(const DeviceAndSwapChain&)				= delete;
		DeviceAndSwapChain& operator=(const DeviceAndSwapChain&)	= delete;

		bool createRenderTargetView();

	private:
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_immediateContext;
		IDXGISwapChain* m_swapChain;

		ID3D11RenderTargetView* m_renderTargetView;

		unsigned int m_bufferCount;
		DXGI_FORMAT m_format;
		DirectX::XMUINT2 m_refreshRate;
		DirectX::XMUINT2 m_size;
		DirectX::XMFLOAT4 m_clearColor;	// 이건 좀 에반가?
		bool m_bWindowed;
		bool m_bVsyncEnabled;
		
		bool m_bInitialized;
	};
}