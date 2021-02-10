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
			bool bMakeMipmaps = static_cast<unsigned int>(m_data.size()) == m_mipLevels;

			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width				= m_width;
			desc.Height				= m_height;
			desc.Format				= m_format;
			desc.Usage				= bMakeMipmaps ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
			desc.BindFlags			= m_bindFlags;
			desc.MipLevels			= m_mipLevels;	// 이걸 여전히 모르겠다. 0이면 set, 1이면 멀티 샘플링된 텍스쳐라는데...
			desc.ArraySize			= m_arraySize;
			desc.SampleDesc.Count	= 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags			= 0;
			desc.CPUAccessFlags		= 0;

			std::vector<D3D11_SUBRESOURCE_DATA> subResources;
			if (bMakeMipmaps)
			{
				for (unsigned int i = 0; i < m_mipLevels; i++)
				{
					D3D11_SUBRESOURCE_DATA data;
					data.pSysMem			= i < static_cast<unsigned int>(m_data.size()) ? m_data[i].data() : nullptr;
					// 계산이 잘못될 수 있다.
					// 차라리 byte per pixel을 곱하는 편이 낫지 않을까?
					data.SysMemPitch		= (m_width >> i) * m_channelCount * (m_bitsPerChannel / 8);	
					data.SysMemSlicePitch	= 0;

					subResources.emplace_back(data);
				}
			}

			if (FAILED(m_renderDevice->CreateTexture2D(&desc, bMakeMipmaps ? subResources.data() : nullptr, &resource)))
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
			desc.Texture2D.MipLevels		= m_data.empty() ? 1 : static_cast<UINT>(m_data.size());
			desc.Texture2D.MostDetailedMip	= 0;

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
