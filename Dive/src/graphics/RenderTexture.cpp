#include "stdafx.h"
#include "RenderTexture.h"
#include "Graphics.h"

namespace Dive
{
	RenderTexture::RenderTexture(uint32_t width, uint32_t height, eDepthFormat depth, DXGI_FORMAT format, bool useMips)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_depthFormat = depth;
		m_useMips = useMips;

		SetName("RenderTexture");
	}

	RenderTexture::~RenderTexture()
	{
		Release();
	}

	bool RenderTexture::Create()
	{
		auto device = Graphics::GetDevice();
		auto deviceContext = Graphics::GetDeviceContext();

		if (m_format == DXGI_FORMAT_UNKNOWN)
		{
			DV_LOG(RenderTexture, err, "[::Create] 잘못된 포멧 설정");
			return false;
		}

		m_mipLevels = CanGenerateMips(m_format) ? (m_useMips ? CalculateMipmapLevels(m_width, m_height) : 1) : 1;

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Format = m_format;
		texDesc.Width = static_cast<UINT>(m_width);
		texDesc.Height = static_cast<UINT>(m_height);
		texDesc.MipLevels = static_cast<UINT>(m_mipLevels);
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.MiscFlags = m_useMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		auto hr = device->CreateTexture2D(&texDesc, nullptr, m_texture.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(RenderTexture, err, "[::Create] CreateTexture2D 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Format = m_format;

		hr = device->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_texture.Get()), &rtvDesc, m_renderTargetView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(RenderTexture, err, "[::Create] CreateRenderTargetView 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC dsvDesc{};
		dsvDesc.Format = m_format;
		dsvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipLevels = m_mipLevels;
		dsvDesc.Texture2D.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_texture.Get()), &dsvDesc, m_shaderResourceView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(RenderTexture, err, "[::Create] CreateShaderResourceView 실패 : {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		if (m_useMips && m_shaderResourceView)	deviceContext->GenerateMips(m_shaderResourceView.Get());
		
		if(m_depthFormat != eDepthFormat::None)
		{
			D3D11_TEXTURE2D_DESC texDesc{};
			texDesc.Format = (m_depthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R24G8_TYPELESS;
			texDesc.Width = static_cast<UINT>(m_width);
			texDesc.Height = static_cast<UINT>(m_height);
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.SampleDesc.Count = 1;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

			hr = device->CreateTexture2D(&texDesc, nullptr, m_depthTexture.GetAddressOf());
			if(FAILED(hr))
			{
				DV_LOG(RenderTexture, err, "[::Create] CreateTexture2D 실패 : {}", ErrorUtils::ToVerbose(hr));
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = (m_depthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			hr = device->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_depthTexture.Get()), &dsvDesc, m_DepthDSV.GetAddressOf());
			if(FAILED(hr))
			{
				DV_LOG(RenderTexture, err, "[::Create] CreateDepthSteniclView 실패 : {}", ErrorUtils::ToVerbose(hr));
				return false;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = (m_depthFormat == eDepthFormat::Depth16) ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			hr = device->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_depthTexture.Get()), &srvDesc, m_depthSRV.GetAddressOf());
			if(FAILED(hr))
			{
				DV_LOG(RenderTexture, err, "[::Create] CreateShaderResourceView 실패 : {}", ErrorUtils::ToVerbose(hr));
				return false;
			}
		}

		return true;
	}

	bool RenderTexture::Resize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		
		return Create();
	}

	void RenderTexture::Release()
	{
		m_depthSRV.Reset();
		m_DepthDSV.Reset();
		m_depthTexture.Reset();
		m_renderTargetView.Reset();

		Texture::Release();
	}
}
