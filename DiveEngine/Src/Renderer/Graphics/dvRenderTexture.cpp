#include "dvRenderTexture.h"

namespace dive
{
	// �ᱹ flag�� format���� �����ؾ� �Ѵ�.
	// �Ʒ��� �����ڴ� ��︮�� �ʴ´�.
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

		// �굵 format�� �޶�����...?

		return false;
	}
	
	bool dvRenderTexture::createRenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
	{
		DV_ASSERT(pDevice != nullptr);

		// format�� �̿��� format�� ������ ���̴�.

		// GBuffer�� ����. Color�� Normal�� ���� ����������, ��ü ������ �����ϴ�.
		// buffer: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT
		// srv: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT
		// rtv: DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R11G11B10_FLOAT

		return false;
	}
	
	bool dvRenderTexture::createDepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
	{
		DV_ASSERT(pDevice != nullptr);

		// depth�� �̿��� format�� ������ ���̴�.

		// GBuffer�� ����. ���� ������ �ٸ���.
		// ������ Spratan�� ��� �� �ٸ��ٴ� �Ŵ�.
		// buffer: DXGI_FORMAT_R24G8_TYPELESS		=> r 24bit, g 8bit�� typeless fortmat 32bit
		// �̿ܿ��� RHI_Format_D32_Float_S8X24_Uint, RHI_Format_D32_Float
		// srv: DXGI_FORMAT_R24_UNORM_X8_TYPELESS	=> depth 24bit�� red�� ǥ���Ѵٴ� �ǹ�
		// �̿ܿ��� DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT
		// dsv: DXGI_FORMAT_D24_UNORM_S8_UINT		=> depth 24bit, stencil 8bit
		// �̿ܿ��� DXGI_FORMAT_D32_FLOAT_S8X24_UINT,DXGI_FORMAT_D32_FLOAT

		// �� ã�ƺ���
		// DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT
		// ������ ������ �� �ִ�.

		return false;
	}
}