#pragma once

namespace Dive
{
	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		ID3D11Device* GetD3DDevice() const;
		ID3D11DeviceContext* GetImmediateContext() const;
		IDXGISwapChain* GetSwapChain() const;

	private:
	private:
	};
}

