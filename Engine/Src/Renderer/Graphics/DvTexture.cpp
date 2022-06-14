#include "divepch.h"
#include "Base/Base.h"
#include "DvTexture.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/GraphicsDevice.h"

namespace Dive
{
	DvTexture::DvTexture()
	{
	}

	DXGI_FORMAT DvTexture::GetSRVFormat(DXGI_FORMAT format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		else if (format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_R16_UNORM;
		else if (format == DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_R32_FLOAT;
		else
			return format;
	}
	
	DXGI_FORMAT DvTexture::GetDSVFormat(DXGI_FORMAT format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		else if (format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_D16_UNORM;
		else if (DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_D32_FLOAT;
		else
			return format;
	}

	uint32_t DvTexture::CalMipMaxLevel(uint32_t width, uint32_t height)
	{
		uint32_t maxLevel = 1;

		while (width > 1 || height > 1)
		{
			++maxLevel;
			width = width > 1 ? (width >> 1u) : 1;
			height = height > 1 ? (height >> 1u) : 1;
		}

		return maxLevel;
	}

	uint32_t DvTexture::GetLevelWidth(uint32_t level) const
	{
		if (level >= m_MipLevels)
			return 0;

		return std::max<uint32_t>(m_Width >> level, 1);
	}

	uint32_t DvTexture::GetLevelHeight(uint32_t level) const
	{
		if (level >= m_MipLevels)
			return 0;

		return std::max<uint32_t>(m_Height >> level, 1);
	}


	uint32_t DvTexture::GetRowDataSize(uint32_t width) const
	{
		switch (m_Format)
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
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT:
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
			return 0;
		}
	}

	void DvTexture::RegenerateMips()
	{
		if (!m_pShaderResourceView)
			return;

		Renderer::GetGraphicsDevice().GetImmediateContext()->GenerateMips(m_pShaderResourceView);

		// dirty check¸¦ Çß´Ù.
	}
}