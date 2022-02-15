#include "divepch.h"
#include "Texture.h"
#include "Base/Base.h"
#include "Dive.h"

namespace Dive
{
	unsigned int GetChannelCount(DXGI_FORMAT format)
	{
		unsigned int channelCount = 0;

		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			channelCount = 4;
			break;
		default:
			break;
		}

		return channelCount;
	}

	Texture2D* Texture2D::Create(unsigned int width, unsigned int height)
	{
		return new Texture2D(width, height);
	}

	Texture2D* Texture2D::Create(const std::string& path)
	{
		return new Texture2D(path);
	}

	Texture2D::Texture2D(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height)
	{
	}
	
	Texture2D::Texture2D(const std::string& path)
		: m_Path(path)
	{
		// 파일 확인
		if (!std::filesystem::exists(path))
		{
			DV_CORE_WARN("{:s}는 존재하지 않는 파일입니다.", path);
			return;
		}

		std::wstring filePath(m_Path.begin(), m_Path.end());
		DirectX::ScratchImage image;

		HRESULT hResult;

		// 흐음... 이게 문제였다니...
		hResult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

		hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
		if (FAILED(hResult))
		{
			DV_CORE_WARN("fail to load image file: {:s}", path);
			return;
		}

		const auto& data = image.GetMetadata();
		m_Width = static_cast<unsigned int>(data.width);
		m_Height = static_cast<unsigned int>(data.height);
		m_Format = data.format;

		auto pDevice = GetRenderer().GetGraphicsDevice()->GetDevice();

		hResult = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_pShaderResourceView);
		if (FAILED(hResult))
		{
			DV_CORE_WARN("ShaderResourceView 생성에 실패하였습니다.");
			return;
		}

		//createResource(image.GetPixels());
	}

	Texture2D::~Texture2D()
	{
	}
	
	bool Texture2D::operator==(const Texture& other)
	{
		return false;
	}

	void Texture2D::createResource(const BYTE* pData)
	{
		if (pData == nullptr)
			return;

		auto pDevice = GetRenderer().GetGraphicsDevice()->GetDevice();
		auto pImmediateContext = GetRenderer().GetGraphicsDevice()->GetImmediateContext();

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.Format = m_Format;
		desc.ArraySize = 1;
		desc.MipLevels = m_bMipmaps ? 0 : 1;
		desc.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;
		desc.BindFlags = m_bMipmaps ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;	// read까지 하려면 uasge가 staging이어야 한다.
		desc.MiscFlags = m_bMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, &m_pResource)))
		{
			DV_CORE_WARN("");
		}

		// 이걸 m_Data에 저장하려면 크기 계산후 할당을 하고
		// memcpy로 복사해야 한다.

		// subresource update
		{
			unsigned int rowPitch = (m_Width * GetChannelCount(m_Format)) * sizeof(unsigned char);

			// mipmap별 get / set을 하려면 map을 통해 타겟을 설정해야 한다.
			pImmediateContext->UpdateSubresource(m_pResource, 0, nullptr, pData, rowPitch, 0);
		}

		// create shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;

			if (FAILED(pDevice->CreateShaderResourceView(m_pResource, &desc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("");
			}
		}

		// generate mipmaps
		if (m_bMipmaps)
		{
			pImmediateContext->GenerateMips(m_pShaderResourceView);

			D3D11_TEXTURE2D_DESC desc;
			m_pResource->GetDesc(&desc);
			m_MipLevels = desc.MipLevels;
		}
	}
}