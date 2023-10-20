#include "divepch.h"
#include "RenderTexture.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	RenderTexture::RenderTexture()
		: Texture(eResourceType::RenderTexture),
		m_pRenderTargetView(nullptr),
		m_pDepthStencilView(nullptr),
		m_bReadOnly(true)
	{
	}

	RenderTexture::~RenderTexture()
	{
	}
	
	bool RenderTexture::SetRenderTarget(int width, int height, DXGI_FORMAT format)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bLinear = true;
		m_Usage = eTextureUsage::RenderTarget;
		m_MipLevels = 1;

		return createResources();
	}
	
	bool RenderTexture::SetDepthStencil(int width, int height, DXGI_FORMAT format, bool readOnly)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bLinear = true;
		m_Usage = eTextureUsage::DepthStencil;
		m_MipLevels = 1;
		m_bReadOnly = readOnly;

		return createResources();
	}

	// Texture2D�� createResource()�� �ʹ� ��ģ��.
	// usage�� ����ؼ� �� �׷���.
	// ��ǻ� dsv�� �߰��� �� ����� �����ϴ�. => readOnly�� dynamic�� �ٸ���?
	bool RenderTexture::createResources()
	{
		DV_RELEASE(m_pTexture2D);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pDepthStencilView);

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = static_cast<UINT>(m_Width);
		texDesc.Height = static_cast<UINT>(m_Height);
		texDesc.Format = m_bLinear ? m_Format : GetSRGBFormat(m_Format);
		texDesc.ArraySize = 1;
		texDesc.MipLevels = m_MipLevels;	// ���� 1�� ����. �Ӹ� ��� ����.
		texDesc.SampleDesc.Count = 1;		// ���� ��Ƽ ���ø� ������.
		texDesc.SampleDesc.Quality = 0;		// ���� ��Ƽ ���ø� ������.
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		// deferred�� ���Ǵ� gbuffer�� srv+rtv, srv+dsv �̷��� ���еǾ�����?
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_Usage == eTextureUsage::RenderTarget)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else if(m_Usage == eTextureUsage::DepthStencil)
			texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		texDesc.MiscFlags = 0;	// ���� �Ӹ� ��� ����.
		texDesc.CPUAccessFlags = 0;

		if (FAILED(Graphics::GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_RELEASE(m_pTexture2D);
			DV_CORE_ERROR("RenderTexture::createResources - Texture2D ������ �����Ͽ����ϴ�.");
			return false;
		}

		// srv
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = GetSRVFormat(texDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;						// �Ӹ� ��� ����.

		if (FAILED(Graphics::GetDevice()->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			DV_RELEASE(m_pShaderResourceView);
			DV_CORE_ERROR("RenderTexture::createResources - ShaderResourceView ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = texDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
			rtvDesc.Texture2D.MipSlice = 0;

			if (FAILED(Graphics::GetDevice()->CreateRenderTargetView(
				static_cast<ID3D11Resource*>(m_pTexture2D),
				&rtvDesc,
				&m_pRenderTargetView)))
			{
				DV_RELEASE(m_pRenderTargetView);
				DV_CORE_ERROR("RenderTexture::createResources - RenderTargetView ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		if (texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {
			DXGI_FORMAT_UNKNOWN,
			D3D11_DSV_DIMENSION_TEXTURE2D,
			0 };

			dsvd.Format = GetDSVFormat(texDesc.Format);
			if (FAILED(Graphics::GetDevice()->CreateDepthStencilView(
				static_cast<ID3D11Resource*>(m_pTexture2D),
				&dsvd,
				&m_pDepthStencilView)))
			{
				DV_CORE_ERROR("RenderTexture::createResources - DepthStencilView ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (m_bReadOnly)
			{
				dsvd.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
				if (FAILED(Graphics::GetDevice()->CreateDepthStencilView(
					static_cast<ID3D11Resource*>(m_pTexture2D),
					&dsvd,
					&m_pDepthStencilViewReadOnly)))
				{
					DV_CORE_ERROR("RenderTexture::createResources - DepthStencilView ReadOnly ������ �����Ͽ����ϴ�.");
					return false;
				}
			}
		}

		return true;
	}
}