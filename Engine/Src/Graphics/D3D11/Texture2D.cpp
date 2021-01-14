#include "DivePch.h"
#include "Texture2D.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "Graphics/Graphics.h"


namespace Dive
{
	Texture2D::Texture2D(Context * context, std::string name)
		: Texture(context)
	{
		m_name = std::move(name);
	}

	Texture2D::~Texture2D()
	{
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_shaderResourceView);
		SAFE_RELEASE(m_resource);
	}
	
	bool Texture2D::CreateRenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format)
	{
		if (width <= 0 || height <= 0)
		{
			CORE_ERROR("");
			return false;
		}

		// base datas
		{
			m_width = width;
			m_height = height;
			m_format = format;

			setViewport(m_width, m_height);
		}

		// texture 생성
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = m_width;
			desc.Height = m_height;
			desc.Format = m_format;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = 0;

			if (FAILED(m_renderDevice->CreateTexture2D(&desc, nullptr, &m_resource)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// renderTargetView 생성
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			desc.Format = m_format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;		// 사용할 mipLevel 같다.

			if (FAILED(m_renderDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_resource), &desc, &m_renderTargetView)))
			{
				CORE_ERROR("");
				SAFE_RELEASE(m_resource);
				return false;
			}
		}

		// shaderResourceView 생성
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = m_format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = -1;
			desc.Texture2D.MostDetailedMip = 0;

			if (FAILED(m_renderDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_resource), &desc, &m_shaderResourceView)))
			{
				CORE_ERROR("");
				SAFE_RELEASE(m_renderTargetView);
				SAFE_RELEASE(m_resource);
				return false;
			}
		}

		return true;
	}

	// stencil 사용 유무에 따라 Format이 DXGI_FORMAT_D32_FLOAT_S8X24_UINT, DXGI_FORMAT_D32_FLOAT 로 나뉜다.
	// 이때 Texture와 ShaderResourceView의 Format도 다르게 설정해야 한다.
	bool Texture2D::CreateDepthStencil(unsigned int width, unsigned int height, bool useStencil)
	{
		if (width <= 0 || height <= 0)
		{
			CORE_ERROR("");
			return false;
		}

		// base datas
		{
			m_width = width;
			m_height = height;
			m_format = useStencil ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT;

			setViewport(m_width, m_height);
		}

		// texture 생성
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = m_width;
			desc.Height = m_height;
			desc.Format = m_format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ? DXGI_FORMAT_R32G8X24_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;	// 이것만 다르다.
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = 0;

			if (FAILED(m_renderDevice->CreateTexture2D(&desc, nullptr, &m_resource)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// DepthStencilView 생성
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			desc.Format = m_format;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			desc.Flags = 0;	// depth, stencil 둘 다 읽고 쓰기 가능

			if (FAILED(m_renderDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_resource), &desc, &m_depthStencilView)))
			{
				CORE_ERROR("");
				SAFE_RELEASE(m_resource);
				return false;
			}
		}

		// shaderResourceView 생성
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = m_format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = -1;
			desc.Texture2D.MostDetailedMip = 0;

			if (FAILED(m_renderDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_resource), &desc, &m_shaderResourceView)))
			{
				CORE_ERROR("");
				SAFE_RELEASE(m_depthStencilView);
				SAFE_RELEASE(m_resource);
				return false;
			}
		}

		return true;
	}
}
