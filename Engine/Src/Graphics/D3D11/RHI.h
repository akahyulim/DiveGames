#pragma once


namespace Dive
{
	class RHI
	{
	public:

		ID3D11Device* GetDevice()					const { return m_device; }
		ID3D11DeviceContext* GetImmediateContext()	const { return m_immediateContext; }
		IDXGISwapChain* GetSwapChain()				const { return m_swapChain; }

	private:
	private:
		ID3D11Device* m_device					= nullptr;;
		ID3D11DeviceContext* m_immediateContext		= nullptr;
		IDXGISwapChain* m_swapChain					= nullptr;
		ID3D11RenderTargetView* m_renderTargetView	= nullptr;
	};
}