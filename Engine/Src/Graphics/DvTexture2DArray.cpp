#include "divepch.h"
#include "DvTexture2DArray.h"
#include "Core/CoreDefs.h"

namespace Dive
{
    DvTexture2DArray::DvTexture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, bool mipChain)
        : m_Format(format)
        , m_ArraySize(arraySize)
    {
        m_Width = width;
        m_Height = height; 
        m_MipLevels = mipChain ? CalculateMipmapLevels(width, height, -1) : 1;

        createColorBuffer();
    }
    
    DvTexture2DArray::DvTexture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, uint32_t mipCount)
        : m_Format(format)
        , m_ArraySize(arraySize)
    {
        m_Width = width;
        m_Height = height;
        m_MipLevels = CalculateMipmapLevels(width, height, mipCount);

        createColorBuffer();
    }

    DvTexture2DArray::DvTexture2DArray(uint32_t width, uint32_t height, uint32_t depth, uint32_t arraySize, bool readOnly)
        : m_ArraySize(arraySize)
    {
        m_Width = width;
        m_Height = height;

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
            DV_ENGINE_ERROR("Texture2DArray 생성 도중 잘못된 깊이 비트를 전달받았습니다.");
            break;
        }

        createDepthBuffer(readOnly);
    }
    
    bool DvTexture2DArray::createColorBuffer()
    {
        // Texture2DArray
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Format = m_Format;
            desc.Width = m_Width;
            desc.Height = m_Height;
            desc.ArraySize = m_ArraySize;
            desc.MipLevels = m_MipLevels;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.MiscFlags = m_MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
            desc.CPUAccessFlags = 0;

            if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
            {
                DV_ENGINE_ERROR("Texture2DArray의 ID3D11Texture2D 생성에 실패하였습니다.");
                DV_RELEASE(m_pTexture);
                return false;
            }
        }

        // RenderTargetView
        {
            D3D11_RENDER_TARGET_VIEW_DESC desc{};
            desc.Format = m_Format;
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.ArraySize = m_ArraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.MipSlice = 0;

            if (FAILED(m_pDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pRenderTargetView)))
            {
                DV_ENGINE_ERROR("Texture2DArray의 ID3D11RenderTargetView 생성에 실패하였습니다.");
                DV_RELEASE(m_pTexture);
                DV_RELEASE(m_pRenderTargetView);
                return false;
            }
        }


        // ShaderResourceView
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = m_Format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.ArraySize = m_ArraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.MipLevels = -1;
            desc.Texture2DArray.MostDetailedMip = 0;

            if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
            {
                DV_ENGINE_ERROR("Texture2DArray의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
                DV_RELEASE(m_pTexture);
                DV_RELEASE(m_pShaderResourceView);
                return false;
            }
        }

        return true;
    }

    bool DvTexture2DArray::createDepthBuffer(bool readOnly)
    {
		// BindFlags만 다르다.
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = m_Format;
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.ArraySize = m_ArraySize;
			desc.MipLevels = m_MipLevels;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = 0;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
			{
				DV_ENGINE_ERROR("Texture2DArray의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				DV_RELEASE(m_pTexture);
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Format = GetDepthStencilViewFormat(m_Format);
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.ArraySize = m_ArraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.MipSlice = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilView)))
			{
				DV_ENGINE_ERROR("Texture2DArray의 ID3D11DepthStencilView 생성에 실패하였습니다.");
				DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pDepthStencilView);
				return false;
			}

			if (readOnly)
			{
				desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

				if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilViewReadOnly)))
				{
					DV_ENGINE_ERROR("Texture2DArray의 ID3D11DepthStencilView(readOnly) 생성에 실패하였습니다.");
					DV_RELEASE(m_pTexture);
					DV_RELEASE(m_pDepthStencilView);
					DV_RELEASE(m_pDepthStencilViewReadOnly);
					return false;
				}
			}
		}

		// 포멧을 따로 전달받아야 한다.
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = GetShaderResourceViewFormat(m_Format);
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.ArraySize = m_ArraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.MipLevels = 1;
            desc.Texture2DArray.MostDetailedMip = 0;

			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
			{
				DV_ENGINE_ERROR("Texture2DArray의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pDepthStencilView);
				DV_RELEASE(m_pDepthStencilViewReadOnly);
				DV_RELEASE(m_pShaderResourceView);
				return false;
			}
		}

        return true;
    }
}