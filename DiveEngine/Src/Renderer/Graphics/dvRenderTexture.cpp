#include "dvRenderTexture.h"
#include "../DiveCore.h"

namespace dive
{
	dvRenderTexture* dvRenderTexture::m_pActive = nullptr;

	bool dvRenderTexture::Create()
	{
		if (m_bCreated)
		{
			CORE_INFO("이미 생성되어 있습니다.");
			return true;
		}

		m_pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		m_pDeviceContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

		// 아래처럼 만드는 것이 아니라
		// Resoure, SRV, RTV, DSV 함수를 따로 만든 후
		// 이들을 구성하는 게 맞다.
		// 그렇다면 dvTexture2D와 동일한 함수를 쓰게 되는 것인가?

		// Color Resource
		if (!createColorResource())
			return false;

		// Depth Stencil Resource
		if (!createDepthStencilResource())
			return false;

		m_bCreated = true;

		return true;
	}
	
	void dvRenderTexture::Release()
	{
		m_pColorResource.Reset();
		m_pColorRenderTargetView.Reset();
		m_pColorShaderResourceView.Reset();

		m_pDepthStencilResource.Reset();
		m_pDepthStencilView.Reset();
		m_pDepthShaderResourceView.Reset();

		m_bCreated = false;
	}

	// Clear까지 하는게 깔끔하다.
	// 그럴려면 Camera를 알아야 한다.
	// 하지만 유니티에는 Camera 변수가 없다.
	// 아마도 Camera에 Clear를 처리하는 구문이 존재하나보다.
	// => 아... 근데 이게 왜 존재하는 걸까? 머리가 복잡해지기 시작했다.
	void dvRenderTexture::SetActive(dvRenderTexture* pRenderTexture)
	{
		if (!pRenderTexture->IsCreated())
		{
			if (!pRenderTexture->Create())
				return;
		}

		m_pActive = pRenderTexture;

		// 좀 더 깔끔하게 정리 필요
		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		auto pRTV = m_pActive ? m_pActive->GetColorRenderTargetView() : Renderer::GetInstance().GetGraphicsDevice()->GetBackbufferRTV();
		auto pDSV = m_pActive ? m_pActive->GetDepthStencilView() : nullptr;

		pImmediateContext->OMSetRenderTargets(1, &pRTV, pDSV);
	}

	bool dvRenderTexture::createColorResource()
	{
		if (m_Format == DXGI_FORMAT_UNKNOWN)
			return true;

		// resource
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.ArraySize = 1;
			desc.MipLevels = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.Format = m_Format;
			desc.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;	// mipmap generate를 생각해야 한다.

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pColorResource.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// srv
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			// cubemap이면 array 였던가...
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;

			if (FAILED(m_pDevice->CreateShaderResourceView(m_pColorResource.Get(), &desc, m_pColorShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// rtv
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			if (FAILED(m_pDevice->CreateRenderTargetView(m_pColorResource.Get(), &desc, m_pColorRenderTargetView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		return true;
	}
	
	bool dvRenderTexture::createDepthStencilResource()
	{
		if (m_Depth == 0)
			return true;

		// resource
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.Format = m_Depth == 16 ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R24G8_TYPELESS;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pDepthStencilResource.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// srv
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Depth == 16 ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			if (FAILED(m_pDevice->CreateShaderResourceView(m_pDepthStencilResource.Get(), &desc, m_pDepthShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// dsv
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Depth == 16 ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D24_UNORM_S8_UINT;

			if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilResource.Get(), &desc, m_pDepthStencilView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		return true;
	}
}