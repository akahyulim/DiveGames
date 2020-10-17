#pragma once

namespace Dive
{
	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		ID3D11Device* GetD3DDevice() const { return m_device.get(); }
		ID3D11DeviceContext* GetImmediateContext() const { return m_immediateContext.get(); }
		IDXGISwapChain* GetSwapChain() const { return m_swapChain.get(); }

	private:
	private:
		std::shared_ptr<ID3D11Device> m_device;
		std::shared_ptr<ID3D11DeviceContext> m_immediateContext;
		std::shared_ptr<IDXGISwapChain> m_swapChain;
	};
}

