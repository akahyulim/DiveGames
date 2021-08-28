#include "dvRenderTexture.h"

namespace dive
{
	// 결국 flag와 format으로 구분해야 한다.
	// 아래의 생성자는 어울리지 않는다.
	dvRenderTexture::dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth)
		: dvTexture(typeid(dvRenderTexture).hash_code(), pDevice)
	{
	}

	dvRenderTexture::dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format)
		: dvTexture(typeid(dvRenderTexture).hash_code(), pDevice)
	{

	}

	bool dvRenderTexture::createShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
	{
		DV_ASSERT(pDevice != nullptr);

		// 얘도 format이 달라지나...?

		return false;
	}
	
	bool dvRenderTexture::createRenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
	{
		DV_ASSERT(pDevice != nullptr);

		// format을 이용해 format을 결정할 것이다.

		// GBuffer의 예다. Color와 Normal에 따라 갈라지지만, 전체 포멧은 동일하다.
		// buffer: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT
		// srv: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT
		// rtv: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT

		return false;
	}
	
	bool dvRenderTexture::createDepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
	{
		DV_ASSERT(pDevice != nullptr);

		// depth를 이용해 format을 결정할 것이다.

		// GBuffer의 예다. 전부 포멧이 다르다.
		// 문제는 Spratan의 경우 또 다르다는 거다.
		// buffer: DXGI_FORMAT_R24G8_TYPELESS		=> r 24bit, g 8bit인 typeless fortmat 32bit
		// 이외에도 RHI_Format_D32_Float_S8X24_Uint, RHI_Format_D32_Float
		// srv: DXGI_FORMAT_R24_UNORM_X8_TYPELESS	=> depth 24bit를 red로 표현한다는 의미
		// 이외에도 DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT
		// dsv: DXGI_FORMAT_D24_UNORM_S8_UINT		=> depth 24bit, stencil 8bit
		// 이외에도 DXGI_FORMAT_D32_FLOAT_S8X24_UINT,DXGI_FORMAT_D32_FLOAT

		// 더 찾아보니
		// DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT
		// 정도로 구성할 수 있다.

		return false;
	}
}