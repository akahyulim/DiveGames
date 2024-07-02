#include "divepch.h"
#include "RenderTexture.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	RenderTexture::RenderTexture()
		: m_pTexture2D(nullptr)
		, m_pShaderResourceView(nullptr)
		, m_pRenderTargetView(nullptr)
		, m_pDepthStencilView(nullptr)
		, m_bDepthReadOnly(false)
	{
	}

	RenderTexture::~RenderTexture()
	{
		Release();
	}

	bool RenderTexture::CreateRenderTargetView(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipMap)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = useMipMap ? 0 : 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = useMipMap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		texDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			Release();
			DV_ENGINE_ERROR("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			Release();
			DV_ENGINE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&rtvDesc,
			&m_pRenderTargetView)))
		{
			Release();
			DV_ENGINE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bUseMipMap = useMipMap;

		return true;
	}

	bool RenderTexture::CreateDepthStencilView(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		texDesc.MiscFlags = 0;
		texDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			Release();
			DV_ENGINE_ERROR("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = GetShaderResourceViewFormat(format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			Release();
			DV_ENGINE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = GetDepthStencilViewFormat(format);
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = m_bDepthReadOnly ? D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL : 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&dsvDesc,
			&m_pDepthStencilView)))
		{
			Release();
			DV_ENGINE_ERROR("DepthStencilView 생성에 실패하였습니다.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		
		return true;
	}
	
	void RenderTexture::Release()
	{
		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}
}