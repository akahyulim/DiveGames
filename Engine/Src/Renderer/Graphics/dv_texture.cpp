#include "divepch.h"
#include "dv_texture.h"
#include "Renderer/Renderer.h"
#include "GraphicsDevice.h"
#include "Base/Base.h"
#include "Resource/Resource.h"		// 나중에 헤더로 옮기기

namespace Dive
{
	static bool CreateTexture2D(
		ID3D11Texture2D*& pTexture,
		unsigned int width,
		unsigned int height,
		unsigned int channelCount,
		unsigned int bitsPerChannel,
		unsigned int flags,
		DXGI_FORMAT format,
		std::vector<TextureSliceData>& data,
		unsigned int mipLevels = 1,
		unsigned int arraySize = 1
	)
	{
		DV_ASSERT(width != 0);
		DV_ASSERT(height != 0);
		DV_ASSERT(mipLevels != 0);
		DV_ASSERT(arraySize != 0);

		const bool hasData = !data.empty() && !data[0].mips.empty() && !data[0].mips[0].bytes.empty();
			
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Width				= width;
		desc.Height				= height;
		desc.MipLevels			= mipLevels;
		desc.ArraySize			= arraySize;
		desc.Format				= format;
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage				= D3D11_USAGE_DEFAULT;		// 일단 지정
		desc.BindFlags			= flags;
		desc.MiscFlags			= 0;
		desc.CPUAccessFlags		= 0;

		std::vector<D3D11_SUBRESOURCE_DATA> subresources;
		if (hasData)
		{
			for (unsigned int arrayIndex = 0; arrayIndex < arraySize; arrayIndex++)
			{
				for (unsigned int mipIndex = 0; mipIndex < mipLevels; mipIndex++)
				{
					auto& subresource = subresources.emplace_back(D3D11_SUBRESOURCE_DATA{});
					subresource.pSysMem				= data[arrayIndex].mips[mipIndex].bytes.data();
					subresource.SysMemPitch			= (width >> mipIndex) * channelCount * (bitsPerChannel / 8);
					subresource.SysMemSlicePitch	= 0;
				}
			}
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);
		
		if (FAILED(pDevice->CreateTexture2D(&desc, hasData ? subresources.data() : nullptr, &pTexture)))
		{
			DV_CORE_ERROR("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	static bool CreateShaderResourceView(
		ID3D11Texture2D* pTexture, 
		ID3D11ShaderResourceView*& pSrv, 
		eResourceType type,
		DXGI_FORMAT format,
		unsigned int mipLevels	= 1,
		unsigned int mostMip	= 0,
		unsigned int arraySize	= 1
	)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = format;
		if (type == eResourceType::Texture2D)
		{
			desc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels			= mipLevels;
			desc.Texture2D.MostDetailedMip		= mipLevels == 1 ? 0 : mostMip;
		}
		else if (type == eResourceType::Texture2dArray)
		{
			desc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize		= arraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipLevels		= mipLevels;
			desc.Texture2DArray.MostDetailedMip = mipLevels == 1 ? 0 : mostMip;
		}
		else if (type == eResourceType::TextureCube)
		{
			desc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels			= mipLevels;
			desc.TextureCube.MostDetailedMip	= mipLevels == 1 ? 0 : mostMip;
		}
		// 사실 cube도 array가 있다.

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateShaderResourceView(pTexture, &desc, &pSrv)))
		{
			DV_CORE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	static bool CreateRenderTargetView(
		ID3D11Texture2D* pTexture,
		ID3D11RenderTargetView* pRtv,
		eResourceType type,
		DXGI_FORMAT format,
		unsigned int arraySize = 1
	)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = format;
		if (type == eResourceType::Texture2D)
		{
			desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice					= 0;
		}
		else if (type == eResourceType::Texture2dArray)
		{
			desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice			= 0;
			desc.Texture2DArray.FirstArraySlice		= 0;			// 일단 스파르탄과 다르게 간다.
			desc.Texture2DArray.ArraySize			= arraySize;
		}
		// RTV는 TextureCube가 없다.

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateRenderTargetView(pTexture, &desc, &pRtv)))
		{
			DV_CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	static bool CreateDepthStencilView(
		ID3D11Texture2D* pTexture,
		ID3D11DepthStencilView* pDsv,
		DXGI_FORMAT format,
		eResourceType type,
		unsigned int arraySize	= 1,
		bool readOnly			= false
	)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = format;
		desc.Flags	= readOnly ? D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL : 0;	// 일단 스텐실도 무조건 포함

		if (type == eResourceType::Texture2D)
		{
			desc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice				= 0;

		}
		else if (type == eResourceType::Texture2dArray)
		{
			desc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice		= 0;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize		= arraySize;	// 이것도 일단 스파르탄이랑 다르게 간다.
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (FAILED(pDevice->CreateDepthStencilView(pTexture, &desc, &pDsv)))
		{
			DV_CORE_ERROR("DepthStencilView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool DV_Texture::createTexture()
	{
		// 멤버 변수를 이용하여 resource, srv, rtv, dsv를 차례대로 만든다.

		if (!CreateTexture2D(m_pTexture, m_Width, m_Height, m_ChannelCount, m_BitsPerChannel, m_Flags, m_Format,
			m_Data, m_MipLevels, m_ArraySize))
		{
			return false;
		}

		// 포멧이 전부 다르다...
		//DXGI_FORMAT formatSrv, formatRtv, formatDsv;

		// 하나의 view에 배열을 생성하는 것인지, view를 배열로 관리해야 하는 것인지 아직 모른다.
		// 일단 cube를 만드는 방법부터 알아보자.

		return true;
	}
}