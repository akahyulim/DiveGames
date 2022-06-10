#include "divepch.h"
#include "dvTexture2D.h"
#include "Base/Base.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/GraphicsDevice.h"

namespace Dive
{
	DvTexture2D::DvTexture2D()
	{
	}

	DvTexture2D::~DvTexture2D()
	{
		Release();
	}
	
	void DvTexture2D::Release()
	{
		// 명시적으로 사용중인 곳을 찾아 nullptr을 전달한다.

		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	// 리소스를 생성하는 함수는 이 것만 공개된다.
	bool DvTexture2D::SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage)
	{
		if (width == 0 || height == 0)
		{
			DV_CORE_ERROR("잘못된 Texture 크기를 전달받았습니다.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_Usage = usage;

		return Create();
	}

	bool DvTexture2D::Create()
	{
		Release();

		if (!m_Width || !m_Height)
			return false;

		// MipLevels 계산
		// 1. 우선 width, height, requestedLevels?를 이용해 직접 계산한다.
		// 2. DSV를 만들면 1로 다시 고정한다.
		// 3. 마지막으로 multiSample == 1, !TEXTURE_DYNAMIC일 경우 앞선 값을, 아니라면 1로 고정한다.
		// TEXTURE_RENDERTARGET, level != 1, multiSample == 1 일 경우 D3D11_RESOURCE_MISC_GENERATE_MIPS를 설정한다.
		// GenerateMips() 함수가 랩핑되어 있다. rendertarget이 그려진 후, 샘플링 전에 호출된다는데...
		// rastertek의 경우 srv를 로드한 후 호출했다. 즉, srv를 사용하기 전 호출하는 듯 하다.

		// texture2d
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = (UINT)m_Width;
		texDesc.Height = (UINT)m_Height;
		texDesc.Format = m_Format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;			// 임시
		texDesc.SampleDesc.Count = 1;	// 임시
		texDesc.SampleDesc.Quality = 0;	// 임시
		texDesc.Usage = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_Usage == eTextureUsage::TEXTURE_RENDERTARGET)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else if (m_Usage == eTextureUsage::TEXTURE_DEPTHSTENCIL)
			texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		// D3D11_SHADER_RESOURCE가 빠질 수 있다.
		texDesc.MiscFlags = 0;	// 임시
		texDesc.CPUAccessFlags = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture 생성에 실패하였습니다.");
			Release();
			return false;
		}

		// srv
		if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = GetSRVFormat(m_Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	// 임시
			srvDesc.Texture2D.MipLevels = 1;		// 임시, -1이면 전체를 사용한다는 의미
			srvDesc.Texture2D.MostDetailedMip = 0;	// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("ShaderReosurceView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// rtv
		if (texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = m_Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// 임시
			rtvDesc.Texture2D.MipSlice = 0;		// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		// dsv
		if (texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));
			dsvDesc.Format = GetDSVFormat(m_Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// 임시
			dsvDesc.Texture2D.MipSlice = 0;	// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_CORE_ERROR("DepthStencilView 생성에 실패하였습니다.");
				Release();
				return false;
			}

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_CORE_ERROR("DepthStencilViewReadOnly 생성에 실패하였습니다.");
				Release();
				return false;
			}
		}

		return true;
	}
}