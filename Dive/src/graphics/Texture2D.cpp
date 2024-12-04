#include "stdafx.h"
#include "Texture2D.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	Texture2D::Texture2D()
	{
		m_Format = DXGI_FORMAT_UNKNOWN;
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, bool mipChain)
	{
		m_Width = width;
		m_Height = height; 
		m_Format = format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(width, height, -1) : 1;

        createColorBuffer();
	}

	// mipCount가 -1일 경우 최대 개수로 생성, 0일 경우 생성하지 않음
	Texture2D::Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount)
 	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_MipLevels = CalculateMipmapLevels(width, height, mipCount);

		createColorBuffer();
	}

	// 유니티의 인터페이스를 참조했지만 세 번째 매개변수가 굳이 depth일 필요가 있나하는 생각이 든다.
	// 그리고 useReadOnly는 Color buffer에도 적용할 수 있어야 하는 것 아닌가 싶다.
	Texture2D::Texture2D(uint32_t width, uint32_t height, uint32_t depth, bool useReadOnly)
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
            DV_LOG(Texture2D, err, "Texture2D 생성 도중 잘못된 깊이 비트를 전달받았습니다.");
            break;
        }

        createDepthBuffer(useReadOnly);
	}

	Texture2D::Texture2D(const std::string& filename, bool mipChain)
	{
		auto file = StringUtils::StringToWString(filename);
		auto ext = FileUtils::GetExtension(filename);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(file.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (ext == ".tga")
		{
			// 색 공간 정보를 포함하지 않는다.
			hResult = DirectX::LoadFromTGAFile(file.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_LOG(Texture2D, err, "Texture2D 생성 과정 중 파일 {:s} 로드에 실패하였습니다.", filename);

		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(m_Width, m_Height, -1) : 1;
		m_bOpaque = img.IsAlphaAllOpaque();

		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileUtils::GetFileName(filename));
	}

	Texture2D::Texture2D(const std::string& filename, uint32_t size, const void* pSource, bool mipChain)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		auto extension = FileUtils::GetExtension(filename);

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
			DV_LOG(Texture2D, err, "{:s} 형식의 메모리 로드에 실패하였습니다.", extension);

		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		m_MipLevels = mipChain ? CalculateMipmapLevels(m_Width, m_Height, -1) : 1;
		m_bOpaque = img.IsAlphaAllOpaque();
		
		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileUtils::GetFileName(filename));
	}

	bool Texture2D::LoadFromFile(const std::string& filename)
	{
		auto file = StringUtils::StringToWString(filename);
		auto ext = FileUtils::GetExtension(filename);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(file.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (ext == ".tga")
		{
			// 색 공간 정보를 포함하지 않는다.
			hResult = DirectX::LoadFromTGAFile(file.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
			DV_LOG(Texture2D, err, "Texture2D 생성 과정 중 파일 {:s} 로드에 실패하였습니다.", filename);

		// 생성자에서 임의로 전달받은 데이터들을 실제 파일 데이터로 갱신
		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		// opaque 유무 img.IsAlphaAllOpaque();
		// 일단 임시로 무조건 생성
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
					DV_LOG(Texture2D, err, "Texture2D의 ID3D11Texture2D 생성에 실패하였습니다.");
					Release();
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
					DV_LOG(Texture2D, err, "Texture2D의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
					Release();
					return false;
				}
			}
		}

		if (!UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch)))
			return false;

		SetName(FileUtils::GetFileName(filename));

		return true;
	}

	bool Texture2D::LoadFromMemory(const std::string& filename, size_t size, const void* pSource)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		auto extension = FileUtils::GetExtension(filename);

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
			DV_LOG(Texture2D, err, "{:s} 형식의 메모리 로드에 실패하였습니다.", extension);

		// 역시 실제 데이터로 갱신
		m_Width = static_cast<uint32_t>(img.GetImages()->width);
		m_Height = static_cast<uint32_t>(img.GetImages()->height);
		m_Format = img.GetImages()->format;
		//m_bOpaque = img.IsAlphaAllOpaque();

		createColorBuffer();

		UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch));

		SetName(FileUtils::GetFileName(filename));

		return true;
	}

	bool Texture2D::UpdateSubresource(const void* pData, uint32_t rowPitch)
	{
		if (!m_pTexture)
		{
			DV_LOG(Texture2D, err, "ID3D11Texture2D 객체가 존재하지 않아 서브 리소스 업데이트에 실패하였습니다.");
			return false;
		}

		if (!pData)
		{
			DV_LOG(Texture2D, err, "비어있는 데이터를 전달받아 서브 리소스 업데이트에 실패하였습니다.");
			return false;
		}

		m_pDeviceContext->UpdateSubresource(static_cast<ID3D11Resource*>(m_pTexture),
			0, nullptr, pData, rowPitch, 0);

		if (m_MipLevels > 1)
			m_pDeviceContext->GenerateMips(m_pShaderResourceView);

		return true;
	}

	bool Texture2D::createColorBuffer()
	{
		// 위치가 조금 애매하다.
		// 허나 생성자 호출 후 LoadFromFile 순으로 진행하려면 이전에 생성한 객체를 제거해야 한다.
		// => 현재 매개변수를 받지 않는 생성자를 되살렸다. ResourceManager에서 객체를 생성하기 위해선 어쩔 수 없었다.
		Release();

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
                DV_LOG(Texture2D, err, "Texture2D의 ID3D11Texture2D 생성에 실패하였습니다.");
				Release();
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
                DV_LOG(Texture2D, err, "Texture2D의 ID3D11RenderTargetView 생성에 실패하였습니다.");
				Release(); 
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
                DV_LOG(Texture2D, err, "Texture2D의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				Release(); 
				return false;
            }
        }

		return true;
	}
	
	bool Texture2D::createDepthBuffer(bool useReadOnly)
	{
		// BindFlags만 다르다.
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
				DV_LOG(Texture2D, err, "Texture2D의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				Release(); 
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
				DV_LOG(Texture2D, err, "Texture2D의 ID3D11DepthStencilView 생성에 실패하였습니다.");
				Release(); 
				return false;
			}

			if (useReadOnly)
			{
				desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

				if (FAILED(m_pDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pDepthStencilViewReadOnly)))
				{
					DV_LOG(Texture2D, err, "Texture2D의 ID3D11DepthStencilView(useReadOnly) 생성에 실패하였습니다.");
					Release(); 
					return false;
				}
			}
		}

		// 포멧을 따로 전달받아야 한다.
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = GetShaderResourceViewFormat(m_Format);
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = m_MipLevels;
			desc.Texture2D.MostDetailedMip = 0;
			
			if (FAILED(m_pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture), &desc, &m_pShaderResourceView)))
			{
				DV_LOG(Texture2D, err, "Texture2D의 ID3D11ShaderResourceView 생성에 실패하였습니다.");
				Release(); 
				return false;
			}
		}

		return true;
	}
}