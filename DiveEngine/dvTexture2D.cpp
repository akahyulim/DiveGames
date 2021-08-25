#include "dvTexture2D.h"
#include "FileSystemHelper.h"
#include "StringHelper.h"
#include "Log.h"

namespace dive
{
	// 유니티에서는 밉맵도 생성했다는데 몇개를 만들어야 할 지 모르겠다.
	// 용도는 색상을 직접 채워넣는 것이다.
	dvTexture2D::dvTexture2D(ID3D11Device* pDevice, unsigned int width, unsigned int height)
		: dvTexture(typeid(dvTexture2D).hash_code(), pDevice)
	{
		m_Width = width;
		m_Height = height;
		m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format				= m_Format;
			desc.Width				= m_Width;
			desc.Height				= m_Height;
			desc.MipLevels			= 1;
			desc.ArraySize			= 1;
			desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
			desc.Usage				= D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
			desc.SampleDesc.Count	= 1;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
			{
				CORE_ERROR("");
				return;
			}
		}

		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format						= m_Format;
			desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels		= 1;
			desc.Texture2D.MostDetailedMip	= 0;

			if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &desc, &m_pSRV)))
			{
				CORE_ERROR("");
				return;
			}
		}
	}

	dvTexture2D::dvTexture2D(ID3D11Device* pDevice, const std::string& filepath, bool hasMipMap)
		: dvTexture(typeid(dvTexture2D).hash_code(), pDevice)
	{
		if (!FileSystemHelper::FileExists(filepath))
		{
			CORE_ERROR("");
			return;
		}

		auto file = StringHelper::Utf8ToUtf16(filepath);
		DirectX::ScratchImage image;

		if (FAILED(DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image)))
		{
			CORE_ERROR("");
			return;
		}

		if (hasMipMap)
		{
			DirectX::ScratchImage mipChain;

			if (FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain)))
			{
				CORE_ERROR("");
				return;
			}


			if (FAILED(DirectX::CreateShaderResourceView(m_pDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &m_pSRV)))
			{
				CORE_ERROR("");
				return;
			}

			setMetadata(mipChain.GetMetadata());
		}
		else
		{
			if (FAILED(DirectX::CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_pSRV)))
			{
				CORE_ERROR("");
				return;
			}

			setMetadata(image.GetMetadata());
		}

		SetName(FileSystemHelper::GetFilenameWithoutExtension(filepath));
	}

	dvTexture2D::~dvTexture2D()
	{
		DV_RELEASE(m_pTexture);
		DV_RELEASE(m_pSRV);
	}

	void dvTexture2D::setMetadata(const DirectX::TexMetadata& data)
	{
		m_Width			= static_cast<unsigned int>(data.width);
		m_Height		= static_cast<unsigned int>(data.height);
		m_Format		= data.format;
		m_MipmapCount	= static_cast<unsigned int>(data.mipLevels);
	}
}