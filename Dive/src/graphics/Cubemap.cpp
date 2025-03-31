#include "stdafx.h"
#include "Cubemap.h"
#include "core/CoreDefs.h"

namespace Dive
{
	Cubemap::Cubemap(UINT32 size, DXGI_FORMAT format, bool useMips)
		: m_Size(size), m_FaceData{}
	{
		m_Format = format;
		m_UseMips = useMips;
	}

	Cubemap::~Cubemap()
	{
		Release();
	}

	void Cubemap::Release()
	{
		DV_RELEASE(m_RenderTargetView);
		Texture::Release();
	}

	void Cubemap::SetFaceData(UINT32 index, const void* pixels, size_t size)
	{
		if (index > 5) 
		{
			DV_LOG(DvCubmap, err, "�߸��� Face �ε���({:d})�� ���޹޾ҽ��ϴ�.", index);
			return;
		}

		m_FaceData[index].resize(size);
		std::copy(static_cast<const uint8_t*>(pixels),
			static_cast<const uint8_t*>(pixels) + size,
			m_FaceData[index].begin());
	}

	bool Cubemap::Create()
	{
		Release();

		m_MipLevels = CanGenerateMips(m_Format) ? (m_UseMips ? CalculateMipmapLevels(m_Size, m_Size) : 1) : 1;

		// texture2d
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = m_Format;
			desc.Width = m_Size;
			desc.Height = m_Size;
			desc.MipLevels = m_MipLevels;
			desc.ArraySize = 6;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | (m_UseMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

			if (FAILED(Graphics::GetDevice()->CreateTexture2D(&desc, nullptr, &m_Texture2D))) 
			{
				DV_LOG(Cubemap, err, "Cubemap�� ID3D11Texture2D ���� ����");
				Release();
				return false;
			}

			if (!m_FaceData[0].empty()) {
				if (std::any_of(m_FaceData.begin(), m_FaceData.end(), [](const auto& data) { return data.empty(); }))
				{
					DV_LOG(Cubemap, err, "�� ť��� Face �����Ͱ� �����մϴ�.");
					Release();
					return false;
				}

				UINT32 rowPitch = m_Size * GetPixelSize(m_Format);
				for (int i = 0; i < 6; ++i) 
				{
					Graphics::GetDeviceContext()->UpdateSubresource(
						m_Texture2D,
						D3D11CalcSubresource(0, static_cast<UINT32>(i), m_MipLevels),
						nullptr,
						(const void*)m_FaceData[i].data(),
						rowPitch,
						rowPitch * m_Size);
				}
			}
		}

		// render target view
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = 6;

			if (FAILED(Graphics::GetDevice()->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_Texture2D), &desc, &m_RenderTargetView)))
			{
				DV_LOG(Cubemap, err, "Cubemap�� ID3D11RenderTargetView ���� ����");
				Release();
				return false;
			}
		}

		// shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = m_MipLevels;

			if (FAILED(Graphics::GetDevice()->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_Texture2D), &desc, &m_ShaderResourceView)))
			{
				DV_LOG(Cubemap, err, "Cubemap�� ID3D11ShaderResourceView ���� ����");
				Release();
				return false;
			}
		}

		if (m_UseMips && m_ShaderResourceView)
			Graphics::GetDeviceContext()->GenerateMips(m_ShaderResourceView);

		return true;
	}

	std::shared_ptr<Cubemap> Cubemap::LoadFromFaceFiles(const std::vector<std::filesystem::path>& faceFilepaths, bool useMips)
	{
		if (faceFilepaths.size() != 6) 
		{
			DV_LOG(Cubemap, err, "�߸��� ť��� �ؽ��� ��θ� ���� �޾ҽ��ϴ�.", faceFilepaths.size());
			return nullptr;
		}

		UINT32 size = 0;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		std::array<std::vector<uint8_t>, 6> faceData;

		for (UINT32 i = 0; i < 6; ++i)
		{
			const auto& filepath = faceFilepaths[i];
			if (!std::filesystem::exists(filepath))
			{
				DV_LOG(Cubemap, err, "ť��� Face �ؽ��� ������ �������� �ʽ��ϴ�: {}", filepath.string());
				return nullptr;
			}

			// �ؽ�ó �ε�
			DirectX::ScratchImage image;
			if (FAILED(DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image)))
			{
				DV_LOG(Cubemap, err, "ť��� Face �ؽ��� �ε� ����: {}", filepath.string());
				return nullptr;
			}

			// ù ��° Face���� ũ��� ���� ����
			if (i == 0)
			{
				size = static_cast<UINT32>(image.GetMetadata().width);
				format = image.GetMetadata().format;
			}

			// ��ȿ�� �˻�
			if (image.GetMetadata().width != size || image.GetMetadata().height != size)
			{
				DV_LOG(Cubemap, err, "ť��� Face ũ�Ⱑ ��ġ���� �ʽ��ϴ�. ({} != {})", image.GetMetadata().width, size);
				return nullptr;
			}

			// Face ������ ����
			const DirectX::Image* img = image.GetImage(0, 0, 0);
			if (!img)
			{
				DV_LOG(Cubemap, err, "ť��� Face �̹��� �����Ͱ� ��ȿ���� �ʽ��ϴ�: {}", filepath.string());
				return nullptr;
			}

			faceData[i].assign(img->pixels, img->pixels + (img->rowPitch * img->height));
		}

		auto cubemap = std::make_shared<Cubemap>(size, format, useMips);
		for (UINT32 i = 0; i < 6; ++i)
			cubemap->SetFaceData(i, faceData[i].data(), faceData[i].size());

		if (!cubemap->Create())
		{
			DV_LOG(Cubemap, err, "ť��� GPU ���ҽ� ���� ����");
			return nullptr;
		}

		return cubemap;
	}
	
	std::shared_ptr<Cubemap> Cubemap::LoadFromFile(const std::filesystem::path& filepath, bool useMips)
	{
		if (!std::filesystem::exists(filepath))
		{
			DV_LOG(Cubemap, err, "ť��� DDS ������ �������� �ʽ��ϴ�: {}", filepath.string());
			return nullptr;
		}

		// DDS ���� �ε�
		DirectX::ScratchImage image;
		if (FAILED(DirectX::LoadFromDDSFile(filepath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image)))
		{
			DV_LOG(Cubemap, err, "ť��� DDS ���� �ε� ����: {}", filepath.string());
			return nullptr;
		}

		// ��Ÿ������ Ȯ��
		const auto& metadata = image.GetMetadata();
		if (!(metadata.miscFlags & DirectX::TEX_MISC_TEXTURECUBE))
		{
			DV_LOG(Cubemap, err, "DDS ������ ť��� ������ �ƴմϴ�: {}", filepath.string());
			return nullptr;
		}

		// ť��� ��ü ����
		auto cubemap = std::make_shared<Cubemap>(static_cast<UINT32>(metadata.width), metadata.format, useMips);

		// �� Face ������ ����
		for (UINT32 i = 0; i < 6; ++i) 
		{
			const DirectX::Image* img = image.GetImage(0, i, 0); // 6���� Face �� �ϳ��� ������
			if (!img)
			{
				DV_LOG(Cubemap, err, "ť��� Face �����Ͱ� ��ȿ���� �ʽ��ϴ�: {}", filepath.string());
				return nullptr;
			}

			cubemap->SetFaceData(i, img->pixels, img->rowPitch * img->height);
		}

		// ť��� ����
		if (!cubemap->Create())
		{
			DV_LOG(Cubemap, err, "ť��� GPU ���ҽ� ���� ����");
			return nullptr;
		}

		return cubemap;
	}
}