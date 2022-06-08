#include "divepch.h"
#include "ImageImporter.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Graphics/Texture2D.h"

namespace Dive
{
	ImageImporter::ImageImporter()
	{
		m_pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(m_pDevice);
	}

	ImageImporter::~ImageImporter()
	{
	}

	// 일단 단일 텍스쳐, 밉맵 생성 가능
	bool ImageImporter::Load(const std::string& filepath, Texture* pTexture, bool generateMips)
	{
		if (!pTexture)
			return false;

		std::wstring tempPath(filepath.begin(), filepath.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		DirectX::ScratchImage baseImage;
		HRESULT hResult = 0;
		if (_wcsicmp(ext, L".dds") == 0)
		{
			hResult = DirectX::LoadFromDDSFile(tempPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, baseImage);
		}
		else if (_wcsicmp(ext, L".tga") == 0)
		{
			hResult = DirectX::LoadFromTGAFile(tempPath.c_str(), nullptr, baseImage);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(tempPath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, baseImage);
		}

		if (FAILED(hResult))
		{
			DV_CORE_ERROR("{:s} 로드에 실패하였습니다.", filepath);
			return false;
		}

		const auto& texMetadata = baseImage.GetMetadata();
		pTexture->SetWidth((const uint32_t)texMetadata.width);
		pTexture->SetHeight((const uint32_t)texMetadata.height);
		pTexture->SetArraySize((const uint32_t)texMetadata.arraySize);
		pTexture->SetFormat((const DXGI_FORMAT)texMetadata.format);
		
		if (generateMips)
		{
			DirectX::ScratchImage mipChain;
			DirectX::GenerateMipMaps(baseImage.GetImages(), baseImage.GetImageCount(), baseImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

			const auto pImages = mipChain.GetImages();
			const auto imageCount = mipChain.GetImageCount();
			for (size_t i = 0; i < imageCount; i++)
			{
				auto rowPitch = pImages[i].rowPitch;
				auto slicePitch = pImages[i].slicePitch;
				auto data = pImages[i].pixels;

				Dive::TextureMip mip;
				mip.rowPitch = (uint32_t)pImages[i].rowPitch;
				mip.slicePitch = (uint32_t)pImages[i].slicePitch;
				//mip.pixels;// = pImages[i].pixels;
				size_t byteSize = rowPitch * slicePitch;
				
				DV_APP_INFO("{0:d} x {1:d}", mip.rowPitch, mip.slicePitch);
				DV_APP_INFO("size: {:d} bytes", (uint32_t)byteSize);
				//mip.pixels.resize(byteSize);
				//auto size = sizeof(pImages[i].pixels);
				//memcpy(&mip.pixels[0], pImages[i].pixels, byteSize);
				
			}

			pTexture->SetMipLevels((const uint32_t)mipChain.GetMetadata().mipLevels);

			mipChain.Release();
		}
		else
		{
			const auto pImages = baseImage.GetImages();
			auto rowPitch = pImages->rowPitch;
			auto slicePitch = pImages->slicePitch;
			auto data = pImages->pixels;

			pTexture->SetMipLevels((const uint32_t)texMetadata.mipLevels);
		}

		baseImage.Release();

		return true;
	}
}