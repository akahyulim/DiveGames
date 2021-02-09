#include "DivePch.h"
#include "Texture2D.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "Graphics/Graphics.h"


namespace Dive
{
	bool Texture2D::createTextureViews(bool readOnly)
	{
		DXGI_FORMAT formatSRV	= m_format;
		DXGI_FORMAT formatRTV	= m_format;
		DXGI_FORMAT formatDSV	= m_format;

		if (m_format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		{
			formatRTV = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}
		else if (m_format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
		{
			formatRTV = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		}
		else if (m_format == DXGI_FORMAT_D32_FLOAT)
		{
			formatRTV = DXGI_FORMAT_R32_FLOAT;
		}

		ID3D11Texture2D* resource = nullptr;

		// texture2d
		{
			// 역시 mipmap과 array에 대한 정확한 이해가 필요하다.

			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width				= m_width;
			desc.Height				= m_height;
			desc.Format				= m_format;
			desc.Usage				= D3D11_USAGE_DEFAULT;
			desc.BindFlags			= m_bindFlags;
			desc.MipLevels			= m_mipLevels;
			desc.ArraySize			= m_arraySize;
			desc.SampleDesc.Count	= 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags			= 0;
			desc.CPUAccessFlags		= 0;

			// miplevel만큼 data가 있다면 subreosurce를 만들어야 한다.
			std::vector<D3D11_SUBRESOURCE_DATA> subResources;
			{

			}

			if (FAILED(m_renderDevice->CreateTexture2D(&desc, subResources.empty() ? nullptr : subResources.data(), &resource)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format						= formatSRV;
			desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels		= -1;
			desc.Texture2D.MostDetailedMip	= 0;
			// 이것도 array size의 영향을 받는다.

			if (FAILED(m_renderDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(resource), &desc, &m_shaderResourceView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		if (IsDepthFormat(m_format))
		{
			// depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			desc.Format				= formatDSV;
			desc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			desc.Flags				= readOnly ? D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL : 0;
			// array size가 있다면 texture2d array가 되어야 한다.

			if (FAILED(m_renderDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(resource), &desc, &m_depthStencilView)))
			{
				CORE_ERROR("");
				return false;
			}

		}
		else
		{
			// render target view
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			desc.Format				= formatRTV;
			desc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			// array size가 있다면 texture2d array가 되어야 한다.

			if (FAILED(m_renderDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(resource), &desc, &m_renderTargetView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		SAFE_RELEASE(resource);

		return true;
	}
}
