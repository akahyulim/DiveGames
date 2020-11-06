#pragma once
#include "RenderDeviceEnums.h"

namespace Dive
{


	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		bool Initialize(HWND hwnd, unsigned int width = 1600, unsigned int height = 900);

		void Present();

		ID3D11Device* GetD3DDevice() const { return m_device.get(); }
		ID3D11DeviceContext* GetImmediateContext() const { return m_immediate_context.get(); }
		IDXGISwapChain* GetSwapChain() const { return m_swap_chain.get(); }

		void ResizeResolution(unsigned int width, unsigned int height);

	private:
		bool createBackBufferRenderTarget();

	private:
		std::shared_ptr<ID3D11Device>			m_device;
		std::shared_ptr<ID3D11DeviceContext>	m_immediate_context;
		std::shared_ptr<IDXGISwapChain>			m_swap_chain;
		ID3D11RenderTargetView*					m_back_buffer_RTV;

		DirectX::XMINT2 m_resolution;
		DirectX::XMINT2 m_refresh_rate;

		bool m_is_windowed;

		unsigned int m_swap_chain_buffer_count;

		eRenderResourceFormat m_format;
	};
}

