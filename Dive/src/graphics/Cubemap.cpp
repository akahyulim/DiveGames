#include "stdafx.h"
#include "Cubemap.h"
#include "Graphics.h"

namespace Dive
{
	Cubemap::Cubemap(uint32_t size, DXGI_FORMAT format, bool useMips)
		: m_Size(size), m_FaceData{}
	{
		m_format = format;
		m_useMips = useMips;

		SetName("Cubemap");
	}

	Cubemap::~Cubemap()
	{
		Release();
	}

	void Cubemap::Release()
	{
		m_renderTargetView.Reset();
		Texture::Release();
	}

	void Cubemap::SetFaceData(uint32_t index, const void* pixels, size_t size)
	{
		if (index > 5) 
		{
			DV_LOG(Cubemap, warn, "[::SetFaceData] 잘못된 Face 인덱스 전달: {}", index);
			return;
		}

		m_FaceData[index].resize(size);
		std::copy(static_cast<const uint8_t*>(pixels), static_cast<const uint8_t*>(pixels) + size, m_FaceData[index].begin());
	}

	bool Cubemap::Create()
	{
		m_mipLevels = CanGenerateMips(m_format) ? (m_useMips ? CalculateMipmapLevels(m_Size, m_Size) : 1) : 1;

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Format = m_format;
		texDesc.Width = static_cast<UINT>(m_Size);
		texDesc.Height = static_cast<UINT>(m_Size);
		texDesc.MipLevels = static_cast<UINT>(m_mipLevels);
		texDesc.ArraySize = 6;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | (m_useMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

		auto hr = Graphics::GetDevice()->CreateTexture2D(&texDesc, nullptr, m_texture.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Cubemap, err, "[::Create] CreateTexture2D 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		if (!m_FaceData[0].empty()) 
		{
			if (std::any_of(m_FaceData.begin(), m_FaceData.end(), [](const auto& data) { return data.empty(); }))
			{
				DV_LOG(Cubemap, err, "[::Create] 빈 규브맵 데이터");
				return false;
			}

			UINT rowPitch = static_cast<UINT>(m_Size * GetPixelSize(m_format));
			for (int i = 0; i < 6; ++i)
			{
				Graphics::GetDeviceContext()->UpdateSubresource(
					m_texture.Get(),
					D3D11CalcSubresource(0, static_cast<UINT>(i), m_mipLevels),
					nullptr,
					(const void*)m_FaceData[i].data(),
					rowPitch,
					rowPitch * m_Size);	// 조금 의심스럽다. 나중에 확인
			}
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = m_format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		rtvDesc.Texture2DArray.ArraySize = 6;

		hr = Graphics::GetDevice()->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_texture.Get()), &rtvDesc, m_renderTargetView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Cubemap, err, "[::Create] CreateRenderTargetView 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = m_format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = m_mipLevels;

		hr = Graphics::GetDevice()->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_texture.Get()), &srvDesc, m_shaderResourceView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Cubemap, err, "[::Create] CreateShaderResourceView 실패 : {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		if (m_useMips && m_shaderResourceView)	Graphics::GetDeviceContext()->GenerateMips(m_shaderResourceView.Get());

		return true;
	}

	std::shared_ptr<Cubemap> Cubemap::LoadFromFaceFiles(const std::vector<std::filesystem::path>& faceFilepaths, bool useMips)
	{
		if (faceFilepaths.size() != 6) 
		{
			DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] 잘못된 경로 전달: {}", faceFilepaths.size());
			return nullptr;
		}

		uint32_t size = 0;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		std::array<std::vector<uint8_t>, 6> faceData;

		for (uint32_t i = 0; i < 6; ++i)
		{
			const auto& filepath = faceFilepaths[i];
			if (!std::filesystem::exists(filepath))
			{
				DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] 존재하지 않는 파일 전달: {}", filepath.string());
				return nullptr;
			}

			// 텍스처 로드
			DirectX::ScratchImage image;
			if (FAILED(DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image)))
			{
				DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] WIC 파일 로드 실패: {}", filepath.string());
				return nullptr;
			}

			// 첫 번째 Face에서 크기와 포맷 결정
			if (i == 0)
			{
				size = static_cast<uint32_t>(image.GetMetadata().width);
				format = image.GetMetadata().format;
			}

			// 유효성 검사
			if (image.GetMetadata().width != size || image.GetMetadata().height != size)
			{
				DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] 잘못된 크기 설정: {} != {}", image.GetMetadata().width, size);
				return nullptr;
			}

			// Face 데이터 저장
			const DirectX::Image* img = image.GetImage(0, 0, 0);
			if (!img)
			{
				DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] 유효하지 않은 이미지 데이터: {}", filepath.string());
				return nullptr;
			}

			faceData[i].assign(img->pixels, img->pixels + (img->rowPitch * img->height));
		}

		auto cubemap = std::make_shared<Cubemap>(size, format, useMips);
		for (uint32_t i = 0; i < 6; ++i)
			cubemap->SetFaceData(i, faceData[i].data(), faceData[i].size());

		if (!cubemap->Create())
		{
			DV_LOG(Cubemap, err, "[::LoadFromFaceFiles] 큐브맵 GPU 리소스 생성 실패");
			return nullptr;
		}

		return cubemap;
	}
	
	std::shared_ptr<Cubemap> Cubemap::LoadFromFile(const std::filesystem::path& filepath, bool useMips)
	{
		if (!std::filesystem::exists(filepath))
		{
			DV_LOG(Cubemap, err, "[::LoadFrmoFile] 존재하지 않는 파일 경로 전달: {}", filepath.string());
			return nullptr;
		}

		// DDS 파일 로드
		DirectX::ScratchImage image;
		if (FAILED(DirectX::LoadFromDDSFile(filepath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image)))
		{
			DV_LOG(Cubemap, err, "[::LoadFrmoFile] DDS 파일 로드 실패: {}", filepath.string());
			return nullptr;
		}

		// 메타데이터 확인
		const auto& metadata = image.GetMetadata();
		if (!(metadata.miscFlags & DirectX::TEX_MISC_TEXTURECUBE))
		{
			DV_LOG(Cubemap, err, "[::LoadFrmoFile] 잘못된 타입 전달: {}", filepath.string());
			return nullptr;
		}

		// 큐브맵 객체 생성
		auto cubemap = std::make_shared<Cubemap>(static_cast<uint32_t>(metadata.width), metadata.format, useMips);

		// 각 Face 데이터 설정
		for (uint32_t i = 0; i < 6; ++i) 
		{
			const DirectX::Image* img = image.GetImage(0, i, 0); // 6개의 Face 중 하나를 가져옴
			if (!img)
			{
				DV_LOG(Cubemap, err, "[::LoadFrmoFile] 유효하지 않은 Face Data: {}", filepath.string());
				return nullptr;
			}

			cubemap->SetFaceData(i, img->pixels, img->rowPitch * img->height);
		}

		// 큐브맵 생성
		if (!cubemap->Create())
		{
			DV_LOG(Cubemap, err, "[::LoadFrmoFile] GPU 리소스 생성 실패");
			return nullptr;
		}

		return cubemap;
	}
}
