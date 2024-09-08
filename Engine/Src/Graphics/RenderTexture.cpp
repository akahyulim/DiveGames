#include "divepch.h"
#include "RenderTexture.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	RenderTexture::RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipmap)
		: m_pColorTexture2D(nullptr)
		, m_pRenderTargetView(nullptr)
		, m_pColorShaderResourceView(nullptr)
		, m_pDepthTexture2D(nullptr)
		, m_pDepthStencilView(nullptr)
		, m_pDepthStencilViewReadOnly(nullptr)
		, m_pDepthShaderResourceView(nullptr)
		, m_ColorFormat(format)
		, m_DepthFormat(DXGI_FORMAT_UNKNOWN)
		, m_ArraySize(1)
		, m_bDepthStencilReadOnly(false)
	{
		m_Width = width;
		m_Height = height;
		m_bUseMipMap = useMipmap;
	}

	RenderTexture::RenderTexture(uint32_t width, uint32_t height, uint32_t depth, bool readOnly)
		: m_pColorTexture2D(nullptr)
		, m_pRenderTargetView(nullptr)
		, m_pColorShaderResourceView(nullptr)
		, m_pDepthTexture2D(nullptr)
		, m_pDepthStencilView(nullptr)
		, m_pDepthStencilViewReadOnly(nullptr)
		, m_pDepthShaderResourceView(nullptr)
		, m_ColorFormat(DXGI_FORMAT_UNKNOWN)
		, m_ArraySize(1)
		, m_bDepthStencilReadOnly(readOnly)
	{
		m_Width = width;
		m_Height = height;

		switch (depth)
		{
		case 16:
			m_DepthFormat = DXGI_FORMAT_R16_TYPELESS;
			break;
		case 24:
			m_DepthFormat = DXGI_FORMAT_R24G8_TYPELESS;
			break;
		case 32:
			m_DepthFormat = DXGI_FORMAT_R32_TYPELESS;
			break;

		default:
			m_DepthFormat = DXGI_FORMAT_UNKNOWN;
			DV_ENGINE_ERROR("잘못된 깊이 비트를 전달받았습니다.");
			break;
		}
	}

	RenderTexture::~RenderTexture()
	{
		Release();
	}

	bool RenderTexture::Create()
	{
		if (m_ColorFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC texDesc{};
			texDesc.Format = m_ColorFormat;
			texDesc.Width = m_Width;
			texDesc.Height = m_Height;
			texDesc.ArraySize = m_ArraySize;
			texDesc.MipLevels = m_bUseMipMap ? 0 : 1;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.MiscFlags = m_bUseMipMap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			texDesc.CPUAccessFlags = 0;

			if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pColorTexture2D)))
			{
				DV_ERROR("생성실패");
				return false;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = m_ColorFormat;
			if (m_ArraySize == 1)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = -1;
				srvDesc.Texture2D.MostDetailedMip = 0;
			}
			else
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.ArraySize = m_ArraySize;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = -1;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
			}

			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pColorTexture2D), &srvDesc, &m_pColorShaderResourceView)))
			{
				DV_ERROR("생성실패");
				return false;
			}

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = m_ColorFormat;
			if (m_ArraySize == 1)
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;
			}
			else
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.ArraySize = m_ArraySize;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.MipSlice = 0;
			}
			if (FAILED(m_pDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_pColorTexture2D), &rtvDesc, &m_pRenderTargetView)))
			{
				DV_ERROR("생성실패");
				return false;
			}
		}

		if (m_DepthFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC texDesc{};
			texDesc.Format = m_DepthFormat;
			texDesc.Width = m_Width;
			texDesc.Height = m_Height;
			texDesc.ArraySize = m_ArraySize;
			texDesc.MipLevels = 1;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			texDesc.MiscFlags = 0;
			texDesc.CPUAccessFlags = 0;

			if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pDepthTexture2D)))
			{
				DV_ERROR("생성실패");
				return false;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = GetShaderResourceViewFormat(m_DepthFormat);
			if (m_ArraySize == 1)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
			}
			else
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.ArraySize = m_ArraySize;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
			}

			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pDepthTexture2D), &srvDesc, &m_pDepthShaderResourceView)))
			{
				DV_ERROR("생성실패");
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = GetDepthStencilViewFormat(m_DepthFormat);
			if (m_ArraySize == 1)
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
			else
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.ArraySize = m_ArraySize;
				dsvDesc.Texture2DArray.FirstArraySlice = 0;
				dsvDesc.Texture2DArray.MipSlice = 0;
			}

			if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pDepthTexture2D), &dsvDesc, &m_pDepthStencilView)))
			{
				DV_ENGINE_ERROR("DepthStencilView 생성에 실패하였습니다.");
				return false;
			}

			if (m_bDepthStencilReadOnly)
			{
				dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

				if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pDepthTexture2D), &dsvDesc, &m_pDepthStencilViewReadOnly)))
				{
					DV_ENGINE_ERROR("DepthStencilViewReadOnly 생성에 실패하였습니다.");
					return false;
				}
			}
		}

		return true;
	}

	void RenderTexture::Release()
	{
		// color
		{
			DV_RELEASE(m_pColorShaderResourceView);
			DV_RELEASE(m_pRenderTargetView);
			DV_RELEASE(m_pColorTexture2D);
		}

		// depth
		{
			DV_RELEASE(m_pDepthShaderResourceView);
			DV_RELEASE(m_pDepthStencilView);
			DV_RELEASE(m_pDepthTexture2D);
		}
	}
}