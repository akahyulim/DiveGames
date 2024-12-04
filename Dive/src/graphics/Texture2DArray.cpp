#include "stdafx.h"
#include "Texture2DArray.h"
#include "Core/CoreDefs.h"

namespace Dive
{
    Texture2DArray::Texture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, bool mipChain)
        : m_ArraySize(arraySize)
    {
        m_Width = width;
        m_Height = height; 
        m_Format = format;
        m_MipLevels = mipChain ? CalculateMipmapLevels(width, height, -1) : 1;

        createColorBuffer();
    }
    
    Texture2DArray::Texture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, uint32_t mipCount)
        : m_ArraySize(arraySize)
    {
        m_Width = width;
        m_Height = height;
        m_Format = format;
        m_MipLevels = CalculateMipmapLevels(width, height, mipCount);

        createColorBuffer();
    }

    Texture2DArray::Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, uint32_t arraySize, bool useReadOnly)
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
            DV_LOG(Texture2DArray, err, "Texture2DArray ���� ���� �߸��� ���� ��Ʈ�� ���޹޾ҽ��ϴ�.");
            break;
        }

        createDepthBuffer(useReadOnly);
    }
    
    bool Texture2DArray::createColorBuffer()
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
                DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11Texture2D ������ �����Ͽ����ϴ�.");
                Release();
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
                DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11RenderTargetView ������ �����Ͽ����ϴ�.");
                Release(); 
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
                DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
                Release(); 
                return false;
            }
        }

        return true;
    }

    bool Texture2DArray::createDepthBuffer(bool useReadOnly)
    {
		// BindFlags�� �ٸ���.
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
				DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
                Release(); 
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
				DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11DepthStencilView ������ �����Ͽ����ϴ�.");
                Release(); 
                return false;
			}

			if (useReadOnly)
			{
				desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

				if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilViewReadOnly)))
				{
					DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11DepthStencilView(useReadOnly) ������ �����Ͽ����ϴ�.");
                    Release(); 
                    return false;
				}
			}
		}

		// ������ ���� ���޹޾ƾ� �Ѵ�.
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
				DV_LOG(Texture2DArray, err, "Texture2DArray�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
                Release(); 
                return false;
			}
		}

        return true;
    }
}