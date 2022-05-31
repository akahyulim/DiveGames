#include "divepch.h"
#include "dv_texture.h"
#include "Renderer/Renderer.h"
#include "GraphicsDevice.h"
#include "Base/Base.h"

namespace Dive
{
	DV_Texture::DV_Texture()
	{
		// device만 획득
	}
	
	DV_Texture::~DV_Texture()
	{
		// 모두 제거
	}

	bool DV_Texture::createTexture()
	{
		DV_ASSERT(m_Width > 0);
		DV_ASSERT(m_Height > 0);

		const bool hasData = !m_Data.empty() && !m_Data[0].mips.empty() && !m_Data[0].mips[0].bytes.empty();

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.MipLevels = m_MipLevels;
		desc.ArraySize = m_ArraySize;
		desc.Format = m_Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;		// 일단 지정
		desc.BindFlags = m_BindFlags;
		desc.MiscFlags = m_MiscFlags;
		desc.CPUAccessFlags = 0;

		std::vector<D3D11_SUBRESOURCE_DATA> subresources;
		if (hasData)
		{
			for (unsigned int arrayIndex = 0; arrayIndex < m_ArraySize; arrayIndex++)
			{
				for (unsigned int mipIndex = 0; mipIndex < m_MipLevels; mipIndex++)
				{
					auto& subresource = subresources.emplace_back(D3D11_SUBRESOURCE_DATA{});
					subresource.pSysMem = m_Data[arrayIndex].mips[mipIndex].bytes.data();
					subresource.SysMemPitch = (m_Width >> mipIndex) * m_ChannelCount * (m_BitsPerChannel / 8);
					subresource.SysMemSlicePitch = 0;
				}
			}
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateTexture2D(&desc, subresources.data(), &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		// srv
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (!createShaderResourceView(0, -1, 0, -1))
				return false;
		}

		// rtv
		if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{

		}

		// dsv
		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{

		}

		return true;
	}

	bool DV_Texture::createShaderResourceView(
		int firstSlice,
		unsigned int sliceCount,
		unsigned int mipCount,
		unsigned int firstMip
		)
	{
		DV_ASSERT(m_pTexture2D != nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;

		switch (m_Format)
		{
		case DXGI_FORMAT_R16_TYPELESS:
			desc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		default:
			desc.Format = m_Format;
			break;
		}

		if (m_ArraySize > 1)
		{
			if (m_MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				if (m_ArraySize > 6)
				{
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.TextureCubeArray.First2DArrayFace = firstSlice;
					desc.TextureCubeArray.MipLevels = mipCount;
					desc.TextureCubeArray.MostDetailedMip = firstMip;
					desc.TextureCubeArray.NumCubes = std::min(m_ArraySize, sliceCount) / 6;
				}
				else
				{
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					desc.TextureCube.MipLevels = mipCount;
					desc.TextureCube.MostDetailedMip = firstMip;
				}
			}
			else
			{
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.ArraySize = sliceCount;
				desc.Texture2DArray.FirstArraySlice = firstSlice;
				desc.Texture2DArray.MipLevels = mipCount;
				desc.Texture2DArray.MostDetailedMip = firstMip;
			}
		}
		else
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = mipCount;
			desc.Texture2D.MostDetailedMip = firstMip;
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateShaderResourceView(m_pTexture2D, &desc, &m_pShaderResourceView)))
		{
			DV_CORE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool DV_Texture::createRenderTargetView(int firstSlice, unsigned int sliceCount, unsigned int mipCount, unsigned int firstMip)
	{
		DV_ASSERT(m_pTexture2D != nullptr);

		D3D11_RENDER_TARGET_VIEW_DESC desc;

		switch (m_Format)
		{
		case DXGI_FORMAT_R16_TYPELESS:
			desc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		default:
			desc.Format = m_Format;
			break;
		}

		if (m_ArraySize > 1)
		{
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize;
			desc.Texture2DArray.FirstArraySlice;
			desc.Texture2DArray.MipSlice;
		}
		else
		{
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice;
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateRenderTargetView(m_pTexture2D, &desc, &m_pRenderTargetView)))
		{
			DV_CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	// 둘 다 사용할 지 전달 필요
	bool DV_Texture::createDepthStencilView()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;

		desc.Flags;

		switch (m_Format)
		{
		case DXGI_FORMAT_R16_TYPELESS:
			desc.Format = DXGI_FORMAT_D16_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			desc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			break;
		default:
			desc.Format = m_Format;
			break;
		}

		if (m_ArraySize > 1)
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize;
			desc.Texture2DArray.FirstArraySlice;
			desc.Texture2DArray.MipSlice;
		}
		else
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice;
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateDepthStencilView(m_pTexture2D, &desc, &m_pDepthStencilView)))
		{
			DV_CORE_ERROR("DepthStencilView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}