#include "divepch.h"
#include "Texture.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
    Texture::Texture()
        : m_pTexture(nullptr)
        , m_pRenderTargetView(nullptr)
        , m_pDepthStencilView(nullptr)
        , m_pDepthStencilViewReadOnly(nullptr)
        , m_pShaderResourceView(nullptr)
        , m_Width(0)
        , m_Height(0)
        , m_MipLevels(1)
		, m_bOpaque(true)
    {
        m_pDevice = Graphics::GetInstance()->GetDevice();
        DV_ASSERT(m_pDevice);

        m_pDeviceContext = Graphics::GetInstance()->GetDeviceContext();
        DV_ASSERT(m_pDeviceContext);
    }
    
    Texture::~Texture()
    {
        DV_RELEASE(m_pRenderTargetView);
        DV_RELEASE(m_pDepthStencilView);
        DV_RELEASE(m_pDepthStencilViewReadOnly);
        DV_RELEASE(m_pShaderResourceView);
    }

    uint32_t Texture::CalculateMipmapLevels(uint32_t width, uint32_t height, int maxLevel)
    {
        if(maxLevel == 0)
            return 1;

        uint32_t levels = static_cast<uint32_t>(std::log2(std::max(width, height))) + 1;

        return maxLevel == -1 ? levels : std::min(levels, static_cast<uint32_t>(maxLevel));
    }

	DXGI_FORMAT Texture::GetShaderResourceViewFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		case DXGI_FORMAT_R16_TYPELESS:
			return DXGI_FORMAT_R16_UNORM;

		case  DXGI_FORMAT_R32_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT;

		default:
			return format;
		}
	}

	DXGI_FORMAT Texture::GetDepthStencilViewFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case  DXGI_FORMAT_R16_TYPELESS:
			return DXGI_FORMAT_D16_UNORM;

		case DXGI_FORMAT_R32_TYPELESS:
			return DXGI_FORMAT_D32_FLOAT;

		default:
			return format;
		}
	}

	uint32_t Texture::CalcuRowPitchSize(uint32_t width, DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_A8_UNORM:
			return width;

		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_R16_TYPELESS:
			return width * 2;

		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
			return width * 4;

		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return width * 8;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return width * 16;

		case DXGI_FORMAT_BC1_UNORM:
			return ((width + 3) >> 2) * 8;

		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
			return ((width + 3) >> 2) * 16;

		default:
			return false;
		}
	}
}