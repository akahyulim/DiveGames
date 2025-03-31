#include "stdafx.h"
#include "Texture2D.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"

namespace Dive
{
	Texture2D::Texture2D(UINT32 width, UINT32 height, DXGI_FORMAT format, bool useMips)
		: m_Width(width), m_Height(height)
	{
		m_Format = format;
		m_UseMips = useMips;
	}

	Texture2D::~Texture2D()
	{
		Release();
	}

	void Texture2D::SetPixelData(const void* pixels, size_t size)
	{
		m_PixelData.resize(size);
		std::copy(static_cast<const uint8_t*>(pixels), static_cast<const uint8_t*>(pixels) + size, m_PixelData.begin());
	}

	bool Texture2D::Create()
	{
		if (m_PixelData.empty())
		{
			DV_LOG(Texture2D, err, "Texture2D 픽셀 데이터 미설정");
			return false;
		}

		Release();

		m_MipLevels = CanGenerateMips(m_Format) ? (m_UseMips ? CalculateMipmapLevels(m_Width, m_Height) : 1) : 1;

		// texture2d
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
			desc.MiscFlags = m_UseMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			desc.CPUAccessFlags = 0;

			if (FAILED(Graphics::GetDevice()->CreateTexture2D(&desc, nullptr, &m_Texture2D)))
			{
				DV_LOG(Texture2D, err, "Texture2D의 ID3D11Texture2D 생성에 실패하였습니다.");
				return false;
			}

			UINT32 rowPitch = m_Width * GetPixelSize(m_Format);
			Graphics::GetDeviceContext()->UpdateSubresource(
				m_Texture2D,
				0, nullptr,
				m_PixelData.data(),
				rowPitch,
				0);
		}

		// ShaderResourceView
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = m_MipLevels;
			desc.Texture2D.MostDetailedMip = 0;

			if (FAILED(Graphics::GetDevice()->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_Texture2D), &desc, &m_ShaderResourceView)))
			{
				DV_LOG(Texture2D, err, "Texture2D의 ID3D11ShaderResourceView 생성 실패");
				Release();
				return false;
			}
		}

		if (m_UseMips && m_ShaderResourceView)
			Graphics::GetDeviceContext()->GenerateMips(m_ShaderResourceView);

		m_PixelData.clear();
		m_PixelData.shrink_to_fit();

		return true;
	}

	std::shared_ptr<Texture2D> Texture2D::LoadFromFile(const std::filesystem::path& filepath, bool useMips)
	{
		auto extension = filepath.extension().string();

		DirectX::ScratchImage img;
		HRESULT result = 0;
		if (extension == ".dds")
			result = DirectX::LoadFromDDSFile(filepath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		else if (extension == ".tga")
			result = DirectX::LoadFromTGAFile(filepath.c_str(), nullptr, img);
		else
			result = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);

		if (FAILED(result))
		{
			DV_LOG(Texture2D, err, "Texture2D 생성 과정 중 파일 {:s} 로드 실패", filepath.string());
			return nullptr;
		}

		auto pNewTexture2D = std::make_shared<Texture2D>(
			static_cast<UINT32>(img.GetImages()->width),
			static_cast<UINT32>(img.GetImages()->height),
			img.GetImages()->format,
			useMips);
		pNewTexture2D->SetPixelData((const void*)img.GetImages()->pixels, img.GetImages()->rowPitch * img.GetImages()->height);
		if (!pNewTexture2D->Create())	
			return nullptr;

		return pNewTexture2D;
	}

	std::shared_ptr<Texture2D> Texture2D::LoadFromMemory(const std::filesystem::path& filepath, size_t size, const void* pSource, bool useMips)
	{
		auto extension = filepath.extension().string();

		DirectX::ScratchImage img;
		HRESULT result = 0;
		if (extension == "dds")
			result = DirectX::LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, nullptr, img);
		else if (extension == "tga")
			result = DirectX::LoadFromTGAMemory(pSource, size, nullptr, img);
		else
			result = DirectX::LoadFromWICMemory(pSource, size, DirectX::WIC_FLAGS_NONE, nullptr, img);

		if (FAILED(result))
		{
			DV_LOG(Texture2D, err, "Texture2D 생성 과정 중 메모리 로드 실패");
			return nullptr;
		}

		auto pNewTexture2D = std::make_shared<Texture2D>(
			static_cast<UINT32>(img.GetImages()->width),
			static_cast<UINT32>(img.GetImages()->height),
			img.GetImages()->format,
			useMips);
		pNewTexture2D->SetPixelData((const void*)img.GetImages()->pixels, img.GetImages()->rowPitch * img.GetImages()->height);
		if (!pNewTexture2D->Create())
			return nullptr;

		return pNewTexture2D;
	}
}
