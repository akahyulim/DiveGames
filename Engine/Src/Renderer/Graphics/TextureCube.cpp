#include "divepch.h"
#include "TextureCube.h"
#include "Base/Base.h"

namespace Dive
{
	TextureCube::TextureCube()
	{
	}

	TextureCube::TextureCube(unsigned long long id)
		: Texture(id)
	{
	}

	bool TextureCube::create()
	{
		Shutdown();

		if (!m_pDevice || !m_Width || !m_Height)
			return false;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Format = m_Format;
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.ArraySize = 6;
		textureDesc.MipLevels = 1;
		textureDesc.BindFlags = m_BindFlags;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		textureDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture2D ������ �����Ͽ����ϴ�.");
			Shutdown();
			return false;
		}

		if (m_BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));

			srvDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = 1;
			srvDesc.TextureCube.MostDetailedMip = 0;

			if (FAILED(m_pDevice->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("ShaderResourceView ������ �����Ͽ����ϴ�.");
				Shutdown();
				return false;
			}
		}

		// urho�� �迭�� �����ߴ�.
		// spartan�� rtv, dsv ��� �迭�� �����ߴ�.
		if (m_BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));

			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.ArraySize = 6;
			rtvDesc.Texture2DArray.FirstArraySlice = 0;

			if (FAILED(m_pDevice->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_CORE_ERROR("RenderTargetView ������ �����Ͽ����ϴ�");
				Shutdown();
				return false;
			}
		}

		// urho�� �������� �ʾҴ�.
		if (m_BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));

			dsvDesc.Format = (DXGI_FORMAT)GetDSVFormat(textureDesc.Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice =0;
			dsvDesc.Texture2DArray.ArraySize = 6;
			dsvDesc.Flags = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_CORE_ERROR("DepthStencilView ������ �����Ͽ����ϴ�.");
				Shutdown();
				return false;
			}

			// ���⿡���� ������ �߻��Ѵ�.
			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			if (FAILED(m_pDevice->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_CORE_ERROR("DepthStencilViewReadOnly ������ �����Ͽ����ϴ�.");
				Shutdown();
				return false;
			}
		}

		// �̰� ��𿡼�...?
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = static_cast<FLOAT>(m_Width);
		m_Viewport.Height = static_cast<FLOAT>(m_Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;


		return true;
	}

	bool TextureCube::SetSize(int size, DXGI_FORMAT format, unsigned int bindFlags)
	{
		if (size <= 0)
		{
			DV_CORE_ERROR("ũ�� ������ �߸��Ǿ����ϴ�.");
			return false;
		}

		m_Width = size;
		m_Height = size;
		m_Format = format;
		m_BindFlags = bindFlags;

		return create();
	}

	bool TextureCube::LoadTexture(const std::string& front, const std::string& back, const std::string& up, const std::string& down, const std::string& left, const std::string& right, unsigned int bindFlags, bool generateMips)
	{
		// �� �ؽ��ĺ� data�� �ʿ��ϴ�.
		// ��, ���긮�ҽ��� �����͸� ������ �����ؾ� �Ѵ�.
		// �Ӹ��� �ʿ��� ��쿣 �� �� �������� �� �ִ�.
		// view�� srv�� �⺻���� �� ���̴�.
		
		return false;
	}
}