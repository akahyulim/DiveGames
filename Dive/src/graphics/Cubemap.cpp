#include "stdafx.h"
#include "Cubemap.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Cubemap::Cubemap(uint32_t size, DXGI_FORMAT format, bool mipChain)
	{
		m_Width = m_Height = size;
		m_Format = format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(size, size, -1) : 1;

		createColorBuffer();
	}

	Cubemap::Cubemap(uint32_t size, uint32_t depth)
	{
		m_Width = m_Height = size;

		switch (depth)
		{
		case 16:
			m_Format = DXGI_FORMAT_R16_TYPELESS;
			break;
		case 24:
			m_Format = DXGI_FORMAT_R24G8_TYPELESS;
			break;
		case 32:
			m_Format = DXGI_FORMAT_R32_TYPELESS;
			break;

		default:
			m_Format = DXGI_FORMAT_UNKNOWN;
			DV_LOG(CubeMap, err, "Cubemap 생성 도중 잘못된 깊이 비트를 전달받았습니다.");
			break;
		}

		createDepthBuffer();
	}

	Cubemap::~Cubemap()
	{
	}
	
	bool Cubemap::createColorBuffer()
	{
		// texture2d
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = m_Format;
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.MipLevels = m_MipLevels;
			desc.ArraySize = 6;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11Texture2D 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// render target view
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = 6;
			
			if (FAILED(m_pDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pRenderTargetView)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11RenderTargetView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = m_MipLevels;

			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		return true;
	}

	bool Cubemap::createDepthBuffer()
	{
		// texture2d
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = m_Format;
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.MipLevels = m_MipLevels;
			desc.ArraySize = 6;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11Texture2D 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// depth stencil view
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Format = GetDepthStencilViewFormat(m_Format);
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = 6;
			desc.Texture2DArray.FirstArraySlice = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilView)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11DepthStencilView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = GetShaderResourceViewFormat(m_Format);
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = m_MipLevels;
			desc.TextureCube.MostDetailedMip = 0;

			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
			{
				DV_LOG(CubeMap, err, "Cubemap의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		return true;
	}
}