#include "stdafx.h"
#include "RenderTexture.h"
#include "core/CoreDefs.h"

namespace Dive
{
	RenderTexture::RenderTexture(UINT32 width, UINT32 height, eDepthFormat depth, DXGI_FORMAT format, bool useMips)
		: m_Width(width), m_Height(height)
	{
		m_Format = format;
		m_DepthFormat = depth;
		m_UseMips = useMips;

		SetName("RenderTexture");
	}

	RenderTexture::~RenderTexture()
	{
		Release();
	}

	bool RenderTexture::Create()
	{
		if (m_Format == DXGI_FORMAT_UNKNOWN)
		{
			DV_LOG(RenderTexture, err, "RenderTexture 객체 데이터 미설정");
			return false;
		}

		Release();

		m_MipLevels = CanGenerateMips(m_Format) ? (m_UseMips ? CalculateMipmapLevels(m_Width, m_Height) : 1) : 1;

		// 현재 코드는 유니티의 RenderTexture api를 따른 것이다.
		// 따라서 depth의 유무에 맞춰 렌더 타겟, 렌더 타겟 + 깊이 버퍼로 생성된다.
		// 그러나 경우에 따라선 깊이 버퍼(+리소스 뷰)만 생성이 필요한 상황도 존재한다.
		// 예를들면 GBuffer의 경우가 그렇다.
		{
			{
				D3D11_TEXTURE2D_DESC desc{};
				desc.Format = m_Format;
				desc.Width = m_Width;
				desc.Height = m_Height;
				desc.MipLevels = m_MipLevels;
				desc.ArraySize = 1;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.MiscFlags = m_UseMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

				if (FAILED(Graphics::GetDevice()->CreateTexture2D(&desc, nullptr, &m_Texture2D)))
				{
					DV_LOG(RenderTexture, err, "RenderTarget Texture 생성 실패");
					return false;
				}
			}

			{
				D3D11_RENDER_TARGET_VIEW_DESC desc{};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;
				desc.Format = m_Format;

				if (FAILED(Graphics::GetDevice()->CreateRenderTargetView(
					static_cast<ID3D11Resource*>(m_Texture2D), &desc, &m_RenderTargetView)))
				{
					DV_LOG(RenderTexture, err, "RenderTarget View 생성 실패");
					Release();
					return false;
				}
			}

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
				desc.Format = m_Format;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipLevels = m_MipLevels;
				desc.Texture2D.MostDetailedMip = 0;

				if (FAILED(Graphics::GetDevice()->CreateShaderResourceView(
					static_cast<ID3D11Resource*>(m_Texture2D), &desc, &m_ShaderResourceView)))
				{
					DV_LOG(RenderTexture, err, "RenderTarget ShaderResourceView 생성 실패");
					Release();
					return false;
				}
			}

			if (m_UseMips && m_ShaderResourceView)
			{
				Graphics::GetDeviceContext()->GenerateMips(m_ShaderResourceView);
			}
		}
		if(m_DepthFormat != eDepthFormat::None)
		{
			{
				D3D11_TEXTURE2D_DESC desc{};
				desc.Format = (m_DepthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R24G8_TYPELESS;
				desc.Width = m_Width;
				desc.Height = m_Height;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

				if (FAILED(Graphics::GetDevice()->CreateTexture2D(&desc, nullptr, &m_DepthStencilTexture2D)))
				{
					DV_LOG(RenderTexture, err, "Depth/Stencil Texture 생성 실패");
					return false;
				}
			}

			{
				D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
				desc.Format = (m_DepthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D24_UNORM_S8_UINT;
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				if (FAILED(Graphics::GetDevice()->CreateDepthStencilView(
					static_cast<ID3D11Resource*>(m_DepthStencilTexture2D), &desc, &m_DepthStencilView)))
				{
					DV_LOG(RenderTexture, err, "Depth/Stencil View 생성 실패");
					Release();
					return false;
				}
			}

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
				desc.Format = (m_DepthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;

				if (FAILED(Graphics::GetDevice()->CreateShaderResourceView(
					static_cast<ID3D11Resource*>(m_DepthStencilTexture2D), &desc, &m_DepthStencilShaderResourceView)))
				{
					DV_LOG(RenderTexture, err, "Depth/Stencil ShaderResourceView 생성 실패");
					Release();
					return false;
				}
			}

		}
		return true;
	}

	bool RenderTexture::Resize(UINT32 width, UINT32 height, eDepthFormat depth, DXGI_FORMAT format, bool useMips)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_DepthFormat = depth;
		
		return Create();
	}

	void RenderTexture::Release()
	{
		DV_RELEASE(m_DepthStencilShaderResourceView);
		DV_RELEASE(m_DepthStencilView);
		DV_RELEASE(m_DepthStencilTexture2D);
		DV_RELEASE(m_RenderTargetView);

		Texture::Release();
	}
}
