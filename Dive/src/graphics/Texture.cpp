#include "stdafx.h"
#include "Texture.h"

namespace Dive
{
	void Texture::Release()
	{
		m_shaderResourceView.Reset();
		m_texture.Reset();
	}

	void Texture::Bind(ID3D11DeviceContext* deviceContext, eShaderResourceSlot slot)
	{
		if (slot == eShaderResourceSlot::Count)
		{
			DV_LOG(Texture, err, "[::Bind] 잘못된 슬롯 지정");
			return;
		}

		deviceContext->PSSetShaderResources(static_cast<UINT>(slot), 1, m_shaderResourceView.GetAddressOf());
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
			DV_LOG("Texture", warn, "[::GetPixelSize] 잘못된 포멧 전달");
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
