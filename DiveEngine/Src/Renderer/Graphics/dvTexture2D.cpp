#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

using namespace std;
using namespace DirectX;

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
		desc.SampleDesc.Count = 1;	// ?
		desc.SampleDesc.Quality = 0;
		desc.Usage;
		desc.CPUAccessFlags;

		// sub resource 
		// 이걸 일단 받아와야지 존재하는지 아닌지 알 수 있다...
		vector<D3D11_SUBRESOURCE_DATA> datas;
		// 단일 혹은 mipmap일 수 있다. 물론 array일수도...

		if (FAILED(pDevice->CreateTexture2D(&desc, datas.data(), ppResource)))
		{
			return false;
		}
		return true;
	}

	bool CreateShaderResourceView(ID3D11Device* pDevice, ID3D11Resource* pResource, ID3D11ShaderResourceView** ppShaderResourceView, DXGI_FORMAT format)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels;
		desc.Texture2D.MostDetailedMip;

		if (FAILED(pDevice->CreateShaderResourceView(pResource, &desc, ppShaderResourceView)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	bool CreateRenderTargetView(ID3D11Device* pDevice, ID3D11Resource* pResource, ID3D11RenderTargetView** ppRenderTargetView, DXGI_FORMAT format)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice;
		
		if (FAILED(pDevice->CreateRenderTargetView(pResource, &desc, ppRenderTargetView)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	bool CreateDepthStencilView(ID3D11Device* pDevice, ID3D11Resource* pResource, ID3D11DepthStencilView** ppDepthStencilView, DXGI_FORMAT format, unsigned int flags)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D;
		desc.Flags = flags;

		if (FAILED(pDevice->CreateDepthStencilView(pResource, &desc, ppDepthStencilView)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	// Resource의 Format으로부터
	// SRV, RTV, DSV의 Format을 정할 수 있어야 한다.
	DXGI_FORMAT GetDepthFormatSRV(DXGI_FORMAT format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		else if (format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		else if (format == DXGI_FORMAT_D32_FLOAT)
			return DXGI_FORMAT_R32_FLOAT;

		return format;
	}

	DXGI_FORMAT GetDepthFormatDSV(DXGI_FORMAT format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		else if (format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		else if (format == DXGI_FORMAT_D32_FLOAT)
			return DXGI_FORMAT_D32_FLOAT;

		return format;
	}

	bool dvTexture2D::CreateColorRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int mipCount)
	{
		Clear();

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_MipCount = mipCount;
		m_BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		// temp : 어떻게 얻어야 할 지 좀 더 생각해보자.
		ID3D11Device* pDevice = nullptr;

		// Resource
		if (!CreateTextureResource(pDevice, m_pResource.GetAddressOf(), m_Width, m_Height, m_Format, m_ArraySize, m_MipCount, m_BindFlags))
			return false;

		// SRV
		if (!CreateShaderResourceView(pDevice, m_pResource.Get(), m_pShaderResourceView.GetAddressOf(), m_Format))
			return false;

		// RTV
		if (!CreateRenderTargetView(pDevice, m_pResource.Get(), m_pRenderTargetView.GetAddressOf(), m_Format))
			return false;

		return true;
	}

	bool dvTexture2D::CreateDepthRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly, unsigned int mipCount)
	{
		Clear();

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_MipCount = mipCount;
		m_BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

		// temp : 어떻게 얻어야 할 지 좀 더 생각해보자.
		ID3D11Device* pDevice = nullptr;

		// Resource
		if (!CreateTextureResource(pDevice, m_pResource.GetAddressOf(), m_Width, m_Height, m_Format, m_ArraySize, m_MipCount, m_BindFlags))
			return false;

		// SRV
		if (!CreateShaderResourceView(pDevice, m_pResource.Get(), m_pShaderResourceView.GetAddressOf(), GetDepthFormatSRV(m_Format)))
			return false;

		// DSV
		{
			if (!CreateDepthStencilView(pDevice, m_pResource.Get(), m_pDepthStencilView.GetAddressOf(), GetDepthFormatDSV(m_Format), 0))
				return false;

			if (readOnly)
			{
				if (!CreateDepthStencilView(pDevice, m_pResource.Get(), m_pDepthStencilViewReadOnly.GetAddressOf(), GetDepthFormatDSV(m_Format), D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL))
					return false;
			}
		}

		return true;
	}

	void dvTexture2D::Clear()
	{
		m_pResource.Reset();
		m_pShaderResourceView.Reset();
		m_pRenderTargetView.Reset();
		m_pDepthStencilView.Reset();
		m_pDepthStencilViewReadOnly.Reset();
	}
}