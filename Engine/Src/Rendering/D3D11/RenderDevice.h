#pragma once
#include "RenderDeviceEnums.h"

namespace Dive
{
	// 생성 및 객체 반환외엔 딱히 하는 일이 없다.
	// 멀티 플랫폼 개발을 위한 구분이었던 것인가?
	class RenderDevice
	{
	public:
		RenderDevice();
		~RenderDevice();

		ID3D11Device* GetD3DDevice() const { return m_D3dDevice; }	// 추후 이름을 바꾸자.
		ID3D11DeviceContext* GetImmediateContext() const { return m_ImmediateContext; }


	private:
		ID3D11Device* m_D3dDevice = nullptr;;
		ID3D11DeviceContext* m_ImmediateContext = nullptr;
	};
}

