#pragma once
#include "RenderDeviceEnums.h"

namespace Dive
{


	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		bool Initialize(HWND hwnd, int width, int height, bool vsync, bool windowed);

		void Present();

		ID3D11Device* GetD3DDevice() const { return m_D3dDevice.get(); }
		ID3D11DeviceContext* GetImmediateContext() const { return m_ImmediateContext.get(); }
		IDXGISwapChain* GetSwapChain() const { return m_SwapChain.get(); }

		void ResizeResolution(unsigned int width, unsigned int height);

	private:
		bool createBackBufferRenderTarget();

	private:
		std::unique_ptr<ID3D11Device>			m_D3dDevice;
		std::unique_ptr<ID3D11DeviceContext>	m_ImmediateContext;
		std::unique_ptr<IDXGISwapChain>			m_SwapChain;
		ID3D11RenderTargetView*					m_BackBuffer;

		DirectX::XMINT2 m_Resolution;
		DirectX::XMINT2 m_RefreshRate;
		bool m_bVSync;
		bool m_bWindowed;

		unsigned int m_swap_chain_buffer_count;

		eRenderResourceFormat m_format;
	};
}

