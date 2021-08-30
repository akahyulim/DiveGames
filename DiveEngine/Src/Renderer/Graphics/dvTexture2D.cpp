#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

namespace dive
{
	// create texture2d
	bool CreateTextureResource(ID3D11Device* pDevice, ID3D11Texture2D** ppResource, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int mipCount,
		unsigned int bindFlags)
	{
		DV_ASSERT(pDevice);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = arraySize; 
		desc.BindFlags = bindFlags;
		desc.Format = format;
		desc.MipLevels = mipCount;
		desc.Usage;

		// sub resource 

		if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, ppResource)))
		{
			return false;
		}
		return true;
	}

	bool CraeteShaderResourceView(ID3D11Device* pDevice, ID3D11ShaderResourceView** ppShaderResourceView, DXGI_FORMAT format)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		return true;
	}

	bool CraeteRenderTargetView(ID3D11Device* pDevice, ID3D11RenderTargetView** ppRenderTargetView, DXGI_FORMAT format)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		// array size가 필요하다...

		return true;
	}

	bool CraeteDepthStencilView(ID3D11Device* pDevice, ID3D11DepthStencilView** ppDepthStencilView, DXGI_FORMAT format)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		return true;
	}

	// 단일 텍스쳐
	// 텍스쳐 배열
	// 그리고 mipmap 및
	// sub rsource 사용시
	// 생성법을 완벽히 숙지해야 한다.
	void dvTexture2D::createResourceAndViews(ID3D11Device* pDevice)
	{
		// Resource
		if (!CreateTextureResource(pDevice, m_pResource.GetAddressOf(), m_Width, m_Height, m_Format, m_ArraySize, m_MipCount, m_BindFlags))
		{
			return;
		}

		// SRV

		if (m_BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			// RTV
		}
		else if (m_BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			// DSV
		}
	}
}