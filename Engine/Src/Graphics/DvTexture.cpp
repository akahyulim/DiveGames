#include "divepch.h"
#include "DvTexture.h"
#include "GraphicsDevice.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	DvTexture::DvTexture(eResourceType type)
		: Resource(type),
		m_pTexture2D(nullptr),
		m_pShaderResourceView(nullptr),
		m_pSamplerState(nullptr),
		m_RequestedMipLevels(0),
		m_MipLevels(1),
		m_bMipLevelDirty(true),
		m_Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR),
		m_AddressMode(D3D11_TEXTURE_ADDRESS_WRAP),
		m_BorderColor(0.0f, 0.0f, 0.0f, 0.0f),
		m_AnisoLevel(1),
		m_bSamplerStateDirty(true)
	{
	}

	DvTexture::~DvTexture()
	{
		DV_RELEASE(m_pSamplerState);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	void DvTexture::SetMipLevelsDirty()
	{
		// rendertargetview가 있어야 한다.
		// urho는 usage로 확인한다.
		if (m_MipLevels > 1)
			m_bMipLevelDirty = true;
	}

	void DvTexture::UpdateMipLevels()
	{
		DV_CORE_ASSERT(GraphicsDevice::IsInitialized ());

		if (m_pShaderResourceView)
		{
			GraphicsDevice::GetDeviceContext()->GenerateMips(m_pShaderResourceView);
			m_bMipLevelDirty = false;
		}
	}
	
	void DvTexture::UpdateSamplerState()
	{
		if (!m_bSamplerStateDirty && m_pSamplerState)
			return;

		DV_RELEASE(m_pSamplerState);

		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = m_Filter;
		desc.AddressU = m_AddressMode;
		desc.AddressV = m_AddressMode;
		desc.AddressW = m_AddressMode;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = m_AnisoLevel;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = m_BorderColor.x;
		desc.BorderColor[1] = m_BorderColor.y;
		desc.BorderColor[2] = m_BorderColor.z;
		desc.BorderColor[3] = m_BorderColor.w;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		if (FAILED(GraphicsDevice::GetDevice()->CreateSamplerState(&desc, &m_pSamplerState)))
		{
			DV_RELEASE(m_pSamplerState);
			DV_CORE_ERROR("DvTexture::UpdateSamplerState - SamplerState 생성에 실패하였습니다.");
			return;
		}

		m_bSamplerStateDirty = false;
	}

	void DvTexture::SetFilter(D3D11_FILTER filter)
	{
		if (m_Filter != filter)
		{
			m_Filter = filter;
			m_bSamplerStateDirty = true;
		}
	}

	void DvTexture::SetAddressMode(D3D11_TEXTURE_ADDRESS_MODE mode)
	{
		if (m_AddressMode != mode)
		{
			m_AddressMode = mode;
			m_bSamplerStateDirty = true;
		}
	}
	
	void DvTexture::SetBorderColor(const DirectX::XMFLOAT4& color)
	{
		if (m_BorderColor.x != color.x || m_BorderColor.y != color.y || m_BorderColor.z != color.z || m_BorderColor.w != color.w)
		{
			m_BorderColor = color;
			m_bSamplerStateDirty = true;
		}
	}
	
	void DvTexture::SetAnisoLevel(int level)
	{
		if (m_AnisoLevel != level)
		{
			m_AnisoLevel = level;
			m_bSamplerStateDirty = true;
		}
	}

	uint32_t DvTexture::GetRowPitchSize(int width) const
	{
		switch (m_Format)
		{
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_A8_UNORM:
			return static_cast<unsigned int>(width);

		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_R16_TYPELESS:
			return static_cast<unsigned int>(width * 2);

		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
			return static_cast<unsigned int>(width * 4);

		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return static_cast<unsigned int>(width * 8);

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return static_cast<unsigned int>(width * 16);

		case DXGI_FORMAT_BC1_UNORM:
			return static_cast<unsigned int>(((width + 3) >> 2) * 8);

		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
			return static_cast<unsigned int>(((width + 3) >> 2) * 16);

		default:
			return 0;
		}
	}

	/*
	void DvTexture::SetViewport(unsigned int index, Viewport* pViewport)
	{
		if (m_Viewports.size() >= index)
			m_Viewports.insert(m_Viewports.begin() + index, pViewport);
		else
			m_Viewports.emplace_back(pViewport);
	}
	*/
	DXGI_FORMAT DvTexture::GetSRGBFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		default:
			return format;
		}
	}
	
	DXGI_FORMAT DvTexture::GetSRVFormat(DXGI_FORMAT format)
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
	
	DXGI_FORMAT DvTexture::GetDSVFormat(DXGI_FORMAT format)
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
	
	int DvTexture::CheckMaxMipLevels(int width, int height, int requestedMipLevels)
	{
		int maxLevel = 1;

		while (width > 1 || height > 1)
		{
			++maxLevel;

			width = width > 1 ? (width >> 1u) : 1;
			height = height > 1 ? (height >> 1u) : 1;
		}

		if (!requestedMipLevels || maxLevel < requestedMipLevels)
			return maxLevel;
		else
			return requestedMipLevels;
	}
}