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

	void DvTexture::RegenerateMips()
	{
		if (!m_pShaderResourceView)
			return;

		Renderer::GetGraphicsDevice().GetImmediateContext()->GenerateMips(m_pShaderResourceView);

		// dirty check¸¦ Çß´Ù.
	}
}