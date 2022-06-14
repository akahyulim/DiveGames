#include "divepch.h"
#include "Image.h"
#include "Base/Base.h"
#include "Resource/Importer/ImageImporter.h"

namespace Dive
{
	Image::~Image()
	{
	}

	// 밉맵은 자동으로 생성해야 할까?
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

		const auto& metaData = img.GetMetadata();
		m_Width = (uint32_t)metaData.width;
		m_Height = (uint32_t)metaData.height;
		m_Format = metaData.format;
		m_ArraySize = (uint32_t)metaData.arraySize;
		m_MipLevels = (uint32_t)metaData.mipLevels;
		metaData.dimension;
		m_bCubeMap = metaData.IsCubemap();
		//m_bUseAlpha = metaData.IsPMAlpha();

		DV_CORE_INFO("META - size: {0:d} x {1:d}, arraySize: {2:d}, mipLevels: {3:d}",
			m_Width, m_Height, m_ArraySize, m_MipLevels);

		// 결국 배열로 저장해야 한다?
		// 배열과 밉맵을 둘로 나누어 저장?
		const auto imageCount = img.GetImageCount();
		const auto* pImages = img.GetImages();
		for (size_t i = 0; i != imageCount; i++)
		{
			auto pImage = pImages[i];

			auto a = pImage.width;
			auto b = pImage.height;
			auto c = pImage.format;
			auto d = pImage.rowPitch;
			pImage.slicePitch;		// 이건 3D에서만...?
			pImage.pixels;

			DV_CORE_INFO("IMAGE - size: {0:d} x {1:d}, rowPitch: {2:d}", a, b, c);
		}

		return true;
	}
}