#include "stdafx.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"

namespace Dive
{
	Texture2D::Texture2D()
	{
		SetName("Texture2D");
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMips)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_useMips = useMips;

		SetName("Texture2D");
	}

	Texture2D::~Texture2D()
	{
		Release();
	}

	void Texture2D::SetPixelData(const void* pixels, size_t size)
	{
		m_pixelData.resize(size);
		std::copy(static_cast<const uint8_t*>(pixels), static_cast<const uint8_t*>(pixels) + size, m_pixelData.begin());
	}

	bool Texture2D::LoadFromFile(const std::filesystem::path& filepath)
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
			DV_LOG(Texture2D, err, "[::LoadFromFile] 파일 읽기 실패: {}", filepath.string());
			return false;
		}

		m_width = static_cast<uint32_t>(img.GetImages()->width);
		m_height = static_cast<uint32_t>(img.GetImages()->height);
		m_format = img.GetImages()->format;
		m_useMips = true;
		
		SetPixelData((const void*)img.GetImages()->pixels, img.GetImages()->rowPitch * img.GetImages()->height);
		if (!Create())
		{
			DV_LOG(Texture2D, err, "[::LoadFromFile] 리소스 생성 실패");
			return false;
		}

		SetFilepath(filepath);

		return true;
	}

	bool Texture2D::Create()
	{
		if (m_pixelData.empty())
		{
			DV_LOG(Texture2D, err, "[::Create] 빈 픽셀 데이터로 시도");
			return false;
		}

		m_mipLevels = CanGenerateMips(m_format) ? (m_useMips ? CalculateMipmapLevels(m_width, m_height) : 1) : 1;

		// texture2d
		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Format = m_format;
		texDesc.Width = static_cast<UINT>(m_width);
		texDesc.Height = static_cast<UINT>(m_height);
		texDesc.ArraySize = 1;
		texDesc.MipLevels = static_cast<UINT>(m_mipLevels);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = m_useMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		texDesc.CPUAccessFlags = 0;

		auto hr = Graphics::GetDevice()->CreateTexture2D(&texDesc, nullptr, m_texture.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Texture2D, err, "[::Create] CreateTexture2D 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		uint32_t rowPitch = m_width * GetPixelSize(m_format);
		Graphics::GetDeviceContext()->UpdateSubresource(
			static_cast<ID3D11Resource*>(m_texture.Get()),
			0, nullptr,
			m_pixelData.data(),
			rowPitch,
			0);

		// ShaderResourceView
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = m_format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(m_mipLevels);
		srvDesc.Texture2D.MostDetailedMip = 0;

		hr = Graphics::GetDevice()->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_texture.Get()), &srvDesc, m_shaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Texture2D, err, "[::Create] CreateShaderReosurceView 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		if (m_useMips && m_shaderResourceView)	Graphics::GetDeviceContext()->GenerateMips(m_shaderResourceView.Get());

		m_pixelData.clear();
		m_pixelData.shrink_to_fit();

		return true;
	}

	bool Texture2D::LoadFromMemory(const std::filesystem::path& filepath, const void* sourceData, size_t size, bool useMips)
	{
		auto extension = filepath.extension().string();

		DirectX::ScratchImage img;
		HRESULT hr = 0;
		if (extension == "dds")
			hr = DirectX::LoadFromDDSMemory((const std::byte*)sourceData, size, DirectX::DDS_FLAGS_NONE, nullptr, img);
		else if (extension == "tga")
			hr = DirectX::LoadFromTGAMemory((const uint8_t*)sourceData, size, nullptr, img);
		else
			hr = DirectX::LoadFromWICMemory((const std::byte*)sourceData, size, DirectX::WIC_FLAGS_NONE, nullptr, img);

		if (FAILED(hr))
		{
			DV_LOG(Texture2D, err, "[::LoadFromMemory] 텍스쳐 메모리 로드 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		m_width = static_cast<uint32_t>(img.GetImages()->width);
		m_height = static_cast<uint32_t>(img.GetImages()->height);
		m_format = img.GetImages()->format;
		m_useMips = useMips;
		SetPixelData((const void*)img.GetImages()->pixels, img.GetImages()->rowPitch * img.GetImages()->height);
		if (!Create())
		{
			DV_LOG(Texture2D, err, "[::LoadFromMemory] 리소스 생성 실패");
			return false;
		}

		return true;
	}
}
