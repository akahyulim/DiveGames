#include "divepch.h"
#include "DvTexture2D.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	DvTexture2D::DvTexture2D()
		: m_Format(DXGI_FORMAT_UNKNOWN)
	{
	}

	DvTexture2D::DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, bool mipChain)
        : m_Format(format)
	{
		m_Width = width;
		m_Height = height; 
		m_MipLevels = mipChain ? CalculateMipmapLevels(width, height, -1) : 1;

        createColorBuffer();
	}

	// mipCount�� -1�� ��� �ִ� ������ ����, 0�� ��� �������� ����
	DvTexture2D::DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount)
        : m_Format(format)
	{
		m_Width = width;
		m_Height = height;
		m_MipLevels = CalculateMipmapLevels(width, height, mipCount);

		createColorBuffer();
	}

	DvTexture2D::DvTexture2D(uint32_t width, uint32_t height, uint32_t depth, bool readOnly)
	{
		m_Width = width;
		m_Height = height;

        switch (depth)
        {
        case 16:
            m_Format = DXGI_FORMAT_R16_TYPELESS;
            break;
        case 24:
            m_Format = DXGI_FORMAT_R24G8_TYPELESS;
            break;
        case 32:
            m_Format = DXGI_FORMAT_R32_TYPELESS;
            break;

        default:
            m_Format = DXGI_FORMAT_UNKNOWN;
            DV_ENGINE_ERROR("Texture2D ���� ���� �߸��� ���� ��Ʈ�� ���޹޾ҽ��ϴ�.");
            break;
        }

        createDepthBuffer(readOnly);
	}

	DvTexture2D::DvTexture2D(const std::string& filename, bool mipChain)
	{
		auto file = StringToWstring(filename);
		auto ext = FileSystem::GetExtension(filename);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(file.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (ext == ".tga")
		{
			// �� ���� ������ �������� �ʴ´�.
			hResult = DirectX::LoadFromTGAFile(file.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_ENGINE_ERROR("Texture2D ���� ���� �� ���� {:s} �ε忡 �����Ͽ����ϴ�.", filename);

		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(m_Width, m_Height, -1) : 1;
		m_bOpaque = img.IsAlphaAllOpaque();

		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileSystem::GetFileName(filename));
	}

	DvTexture2D::DvTexture2D(const std::string& filename, uint32_t size, const void* pSource, bool mipChain)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		auto extension = FileSystem::GetExtension(filename);

		if (extension == "dds")
		{
			hResult = DirectX::LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (extension == "tga")
		{
			hResult = DirectX::LoadFromTGAMemory(pSource, size, nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICMemory(pSource, size, DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_ENGINE_ERROR("{:s} ������ �޸� �ε忡 �����Ͽ����ϴ�.", extension);

		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(m_Width, m_Height, -1) : 1;
		m_bOpaque = img.IsAlphaAllOpaque();
		
		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileSystem::GetFileName(filename));
	}

	bool DvTexture2D::LoadFromFile(const std::string& filename)
	{
		auto file = StringToWstring(filename);
		auto ext = FileSystem::GetExtension(filename);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(file.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (ext == ".tga")
		{
			// �� ���� ������ �������� �ʴ´�.
			hResult = DirectX::LoadFromTGAFile(file.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_ENGINE_ERROR("Texture2D ���� ���� �� ���� {:s} �ε忡 �����Ͽ����ϴ�.", filename);

		// �����ڿ��� ���Ƿ� ���޹��� �����͵��� ���� ���� �����ͷ� ����
		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		// opaque ���� img.IsAlphaAllOpaque();
		// �ϴ� �ӽ÷� ������ ����
		m_MipLevels = CalculateMipmapLevels(m_Width, m_Height);

		//if (!createColorBuffer())
		//	return false;

		{
			// Texture2D
			{
				D3D11_TEXTURE2D_DESC desc{};
				desc.Format = m_Format;
				desc.Width = m_Width;
				desc.Height = m_Height;
				desc.ArraySize = 1;
				desc.MipLevels = m_MipLevels;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				desc.MiscFlags = m_MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
				desc.CPUAccessFlags = 0;

				if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
				{
					DV_ENGINE_ERROR("Texture2D�� ID3D11Texture2D ������ �����Ͽ����ϴ�.");
					DV_RELEASE(m_pTexture);
					return false;
				}
			}

			// ShaderResourceView
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
				desc.Format = m_Format;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipLevels = m_MipLevels;
				desc.Texture2D.MostDetailedMip = 0;

				if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
				{
					DV_ENGINE_ERROR("Texture2D�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
					DV_RELEASE(m_pTexture);
					DV_RELEASE(m_pShaderResourceView);
					return false;
				}
			}
		}

		if (!UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch)))
			return false;

		SetName(FileSystem::GetFileName(filename));

		return true;
	}

	bool DvTexture2D::LoadFromMemory(const std::string& filename, size_t size, const void* pSource)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		auto extension = FileSystem::GetExtension(filename);

		if (extension == "dds")
		{
			hResult = DirectX::LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (extension == "tga")
		{
			hResult = DirectX::LoadFromTGAMemory(pSource, size, nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICMemory(pSource, size, DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_ENGINE_ERROR("{:s} ������ �޸� �ε忡 �����Ͽ����ϴ�.", extension);

		// ���� ���� �����ͷ� ����
		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		//m_bOpaque = img.IsAlphaAllOpaque();

		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileSystem::GetFileName(filename));

		return true;
	}

	bool DvTexture2D::UpdateSubresource(const void* pData, uint32_t rowPitch)
	{
		if (!m_pTexture)
		{
			DV_ENGINE_ERROR("ID3D11Texture2D ��ü�� �������� �ʾ� ���� ���ҽ� ������Ʈ�� �����Ͽ����ϴ�.");
			return false;
		}

		if (!pData)
		{
			DV_ENGINE_ERROR("����ִ� �����͸� ���޹޾� ���� ���ҽ� ������Ʈ�� �����Ͽ����ϴ�.");
			return false;
		}

		m_pDeviceContext->UpdateSubresource(static_cast<ID3D11Resource*>(m_pTexture),
			0, nullptr, pData, rowPitch, 0);

		if (m_MipLevels > 1)
			m_pDeviceContext->GenerateMips(m_pShaderResourceView);

		return true;
	}

	bool DvTexture2D::createColorBuffer()
	{
		// ��ġ�� ���� �ָ��ϴ�.
		// �㳪 ������ ȣ�� �� LoadFromFile ������ �����Ϸ��� ������ ������ ��ü�� �����ؾ� �Ѵ�.
		// => ���� �Ű������� ���� �ʴ� �����ڸ� �ǻ�ȴ�. ResourceManager���� ��ü�� �����ϱ� ���ؼ� ��¿ �� ������.
		DV_RELEASE(m_pTexture);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pShaderResourceView);

        // Texture2D
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Format = m_Format;
            desc.Width = m_Width;
            desc.Height = m_Height;
            desc.ArraySize = 1;
            desc.MipLevels = m_MipLevels;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.MiscFlags = m_MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
            desc.CPUAccessFlags = 0;

            if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
            {
                DV_ENGINE_ERROR("Texture2D�� ID3D11Texture2D ������ �����Ͽ����ϴ�.");
				DV_RELEASE(m_pTexture);
				return false;
            }
        }

        // RenderTargetView
        {
            D3D11_RENDER_TARGET_VIEW_DESC desc{};
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = 0;

            if (FAILED(m_pDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pRenderTargetView)))
            {
                DV_ENGINE_ERROR("Texture2D�� ID3D11RenderTargetView ������ �����Ͽ����ϴ�.");
                DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pRenderTargetView);
                return false;
            }
        }

        
        // ShaderResourceView
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = m_MipLevels;
            desc.Texture2D.MostDetailedMip = 0;

            if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
            {
                DV_ENGINE_ERROR("Texture2D�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
                DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pShaderResourceView);
                return false;
            }
        }

		return true;
	}
	
	bool DvTexture2D::createDepthBuffer(bool readOnly)
	{
		// BindFlags�� �ٸ���.
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = m_Format;
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.ArraySize = 1;
			desc.MipLevels = m_MipLevels;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = 0;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
			{
				DV_ENGINE_ERROR("Texture2D�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
				DV_RELEASE(m_pTexture);
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Format = GetDepthStencilViewFormat(m_Format);
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilView)))
			{
				DV_ENGINE_ERROR("Texture2D�� ID3D11DepthStencilView ������ �����Ͽ����ϴ�.");
				DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pDepthStencilView);
				return false;
			}

			if (readOnly)
			{
				desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

				if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilViewReadOnly)))
				{
					DV_ENGINE_ERROR("Texture2D�� ID3D11DepthStencilView(readOnly) ������ �����Ͽ����ϴ�.");
					DV_RELEASE(m_pTexture);
					DV_RELEASE(m_pDepthStencilView);
					DV_RELEASE(m_pDepthStencilViewReadOnly);
					return false;
				}
			}
		}

		// ������ ���� ���޹޾ƾ� �Ѵ�.
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = GetShaderResourceViewFormat(m_Format);
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = m_MipLevels;
			desc.Texture2D.MostDetailedMip = 0;
			
			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
			{
				DV_ENGINE_ERROR("Texture2D�� ID3D11ShaderResourceView ������ �����Ͽ����ϴ�.");
				DV_RELEASE(m_pTexture);
				DV_RELEASE(m_pDepthStencilView);
				DV_RELEASE(m_pDepthStencilViewReadOnly);
				DV_RELEASE(m_pShaderResourceView);
				return false;
			}
		}

		return true;
	}
}