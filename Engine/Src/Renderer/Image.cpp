#include "divepch.h"
#include "Image.h"
#include "Base/Base.h"

namespace Dive
{
	Image::~Image()
	{
	}

	// 추후 이름을 정리해야 한다.
	void Image::Reset()
	{
		// mip, array 다 제거해야 한다.
	}

	// 밉맵은 자동으로 생성해야 할까?
	// 생각해보니 이것두 에바네... Image Import를 사용하지 않는다.
	// 이거랑 ImageImporter를 지우고 Texture Class에서 DirectXTex를 직접 사용하도록 하자.
	bool Image::Load(const std::string& filepath)
	{
		DirectX::ScratchImage img;

		std::wstring tempPath(filepath.begin(), filepath.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		HRESULT hResult = 0;
		if (_wcsicmp(ext, L".dds") == 0)
		{
			hResult = DirectX::LoadFromDDSFile(tempPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (_wcsicmp(ext, L".tga") == 0)
		{
			hResult = DirectX::LoadFromTGAFile(tempPath.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(tempPath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_CORE_ERROR("{:s} 로드에 실패하였습니다.", filepath);
			return false;
		}

		// 1. 특정 파일은 array와 mipmap을 함께 가진다.
		// 2. 즉, array와 mipmap을 구분하여 저장해야 한다?
		// 3. 단일 array, mipLevel이라도 mipmap을 생성해야 한다?

		const auto arraySize = img.GetMetadata().arraySize;
		const auto mipLevels = img.GetMetadata().mipLevels;
		const auto imageCount = img.GetImageCount();
		
		m_bCubeMap = img.GetMetadata().IsCubemap();
		if (!m_bCubeMap && arraySize > 1)
		{
			m_bArray = true;
		}

		auto pCurrentImage = this;
		auto pCurrentMip = this;

		for (size_t arrayIndex = 0; arrayIndex < arraySize; arrayIndex++)
		{
			pCurrentMip = pCurrentImage;

			for (size_t mipLevel = 0; mipLevel < mipLevels; mipLevel++)
			{
				auto pImage = img.GetImage(mipLevel, arrayIndex, 0);

				pCurrentMip->m_bCubeMap = m_bCubeMap;
				pCurrentMip->m_bArray = m_bArray;
				pCurrentMip->m_Format = pImage->format;
				pCurrentMip->m_ArraySize = (uint32_t)arraySize;
				pCurrentMip->m_MipLevels = (uint32_t)mipLevels;
				pCurrentMip->m_Width = (uint32_t)pImage->width;
				pCurrentMip->m_Height = (uint32_t)pImage->height;

				auto size = pImage->width * pImage->height * DirectX::BitsPerPixel(pImage->format);
				DV_CORE_INFO("each size: {0:d}", size);

				// data
				//m_pData = new uint8_t[pImage->rowPitch * pImage->height];
				//memcpy(m_pData, pImage->pixels, sizeof(uint8_t) * pImage->rowPitch * pImage->height);

				if (mipLevel + 1 < mipLevels)
				{
					auto pNextLevel = new Image;
					pCurrentMip->m_pNextLevel = pNextLevel;
					pCurrentMip = pNextLevel;
				}
			}

			if (arrayIndex + 1 < arraySize)
			{
				auto pNextSibling = new Image;
				pCurrentImage->m_pNextSibling = pNextSibling;
				pCurrentImage = pNextSibling;
			}
		}

		//DV_CORE_INFO("array: {:s}", m_bArray ? "true" : "false");
		//DV_CORE_INFO("cubemap: {:s}", m_bCubeMap ? "true" : "false");
		//DV_CORE_INFO("volummap: {:s}", img.GetMetadata().IsVolumemap() ? "true" : "false");

		DV_CORE_INFO("total size: {:d}", img.GetPixelsSize());

		return true;
	}
}