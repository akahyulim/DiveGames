#pragma once
#include "RenderDeviceEnums.h"

namespace Dive
{


	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		ID3D11Device* GetD3DDevice() const { return m_D3dDevice; }
		ID3D11DeviceContext* GetImmediateContext() const { return m_ImmediateContext; }


	private:
		ID3D11Device* m_D3dDevice = nullptr;;
		ID3D11DeviceContext* m_ImmediateContext = nullptr;
	};
}

