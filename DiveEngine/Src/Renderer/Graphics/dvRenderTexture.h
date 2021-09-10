#pragma once
#include "dvTexture.h"

namespace dive
{
	// 유니티를 따라가는게 나을 것 같다.
	// 결국 이건 rasterTek의 2D Rednering과 동일하다.
	// 카메라가 비추는 걸 텍스쳐에 따로 그린다.(카메라가 2개 이상인 이유)
	// 기본적으로 color와 depth(&stencil)을 모두 가지며
	// cube map으로 구성할 수 있고, mipmap 생성도 지원할 수 있어야 한다.
	// 참고1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// 참고2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// 참고3: https://www.rastertek.com/dx11tut11.html
	class dvRenderTexture : public dvTexture
	{
	public:
		dvRenderTexture(unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			// depth steicil view는 depth로 format을 정한다.
			// 그런데... color와 depth를 모두 가진다면 결국 depth용 srv도 필요한 거 아닌가?
			// 이 부분은 rasterTek의 2D Rendering을 분석 후 좀 더 생각해보자.
		}

	private:

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	};
}