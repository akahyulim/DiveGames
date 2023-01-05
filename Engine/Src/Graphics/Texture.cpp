#include "divepch.h"
#include "Texture.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Renderer/Viewport.h"
#include "IO/Log.h"

namespace Dive
{
	Texture::Texture(Context* pContext)
		: Resource(pContext),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_Format(DXGI_FORMAT_UNKNOWN),
		m_Usage(eTextureUsage::Static),
		m_Width(0),
		m_Height(0),
		m_Depth(0),
		m_MipmapCount(0),
		m_bEnableMipmap(true),
		m_bMipLevelsDirty(false),	// 이것두 true가 디폴트인가?
		m_pTexture2D(nullptr),
		m_pShaderResourceView(nullptr),
		m_pRenderTargetView(nullptr),
		m_pDepthStencilView(nullptr),
		m_pDepthStencilViewReadOnly(nullptr),
		m_bParametersDirty(false)	// 추후 true로...
	{
		DV_ASSERT(m_pGraphics);
	}

	Texture::~Texture()
	{
	}

	void Texture::SetMipLevelsDirty()
	{
		if (m_Usage == eTextureUsage::RenderTarget && m_MipmapCount > 1)
			m_bMipLevelsDirty = true;
	}

	// Graphics의 SetTexture에서 호출된다.
	void Texture::GenerateLevels()
	{
		if (!m_pShaderResourceView)
			return;

		m_pGraphics->GetDeviceContext()->GenerateMips(m_pShaderResourceView);
	}

	void Texture::UpdateParameters()
	{
		if (!m_bParametersDirty && m_pSampler)
			return;

		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter;
		desc.AddressU;
		desc.AddressV;
		desc.AddressW;
		desc.MipLODBias;
		desc.MaxAnisotropy;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.BorderColor[0];
		desc.BorderColor[1];
		desc.BorderColor[2];
		desc.BorderColor[3];
		desc.MinLOD;
		desc.MaxLOD;
		if (FAILED(m_pGraphics->GetDevice()->CreateSamplerState(&desc, &m_pSampler)))
		{
			DV_RELEASE(m_pSampler);
			DV_LOG_ENGINE_ERROR("Texture::UpdateParameters - Sampler 생성에 실패하였습니다.");
			return;
		}

		m_bParametersDirty = false;
	}

	unsigned int Texture::GetRowPitchSize(int width) const
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

	DXGI_FORMAT Texture::GetSRVFormat(DXGI_FORMAT format)
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

	DXGI_FORMAT Texture::GetDSVFormat(DXGI_FORMAT format)
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

	unsigned int Texture::CheckMaxMipmapCount(int width, int height)
	{
		unsigned int maxLevel = 1;
	
		while (width > 1 || height > 1)
		{
			++maxLevel;

			width = width > 1 ? (width >> 1u) : 1;
			height = height > 1 ? (height >> 1u) : 1;
		}

		return maxLevel;
	}

	void Texture::SetViewport(unsigned int index, Viewport* pViewport)
	{
		if (m_Viewports.size() >= index)
			m_Viewports.insert(m_Viewports.begin() + index, pViewport);
		else
			m_Viewports.emplace_back(pViewport);
	}
}