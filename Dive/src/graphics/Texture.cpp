#include "stdafx.h"
#include "Texture.h"
#include "core/CoreDefs.h"

namespace Dive
{
	void Texture::Release()
	{
		DV_RELEASE(m_ShaderResourceView);
		DV_RELEASE(m_Texture2D);
	}

	uint32_t Texture::CalculateMipmapLevels(uint32_t width, uint32_t height)
	{
		return std::max(1u, static_cast<uint32_t>(std::log2(std::max(width, height))) + 1);
	}

	uint32_t Texture::GetPixelSize(DXGI_FORMAT format)
	{
		switch (format) 
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_R32_FLOAT:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return 8;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return 16;
		default:
			DV_LOG("Texture", warn, "지원되지 않는 DXGI_FORMAT을 전달받았습니다.");
			return 0;
		}
	}

	bool Texture::CanGenerateMips(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
			return false;
		default:
			return true;
		}
	}
}
