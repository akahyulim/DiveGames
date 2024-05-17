#include "divepch.h"
#include "Image.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"

namespace Dive
{
	Image::Image()
		: m_Width(0),
		m_Height(0),
		m_Depth(0),
		m_Format(DXGI_FORMAT_R8G8B8A8_UNORM),
		m_bCubemap(false)
	{
	}

	bool Image::LoadFromFile(const std::string& fileName)
	{
		auto filePath = StringToWstring(fileName);
		auto ext = FileSystem::GetExtension(fileName);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			//hResult = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
			hResult = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, nullptr, img);
		}
		else if (ext == ".tga")
		{
			hResult = DirectX::LoadFromTGAFile(filePath.c_str(), nullptr, img);
		}
		else
		{
			// https://github.com/microsoft/DirectXTex/wiki/WIC-I-O-Functions
			//hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
			hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, nullptr, img);
			//hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_LINEAR , nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_ENGINE_ERROR("Image::LoadFromFile - {:s} 로드에 실패하였습니다.", fileName);
			return false;
		}

		loadScrachImage(img);
		img.Release();

		return true;
	}

	bool Image::LoadFromMemory(const std::string& extension, size_t size, const void* pSrcData)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (extension == "dds")
			hResult = DirectX::LoadFromDDSMemory(pSrcData, size, DirectX::DDS_FLAGS_FORCE_RGB, nullptr, img);
		else if (extension == "tga")
			hResult = DirectX::LoadFromTGAMemory(pSrcData, size, nullptr, img);
		else
			hResult = DirectX::LoadFromWICMemory(pSrcData, size, DirectX::WIC_FLAGS_FORCE_RGB, nullptr, img);

		if (FAILED(hResult))
		{
			DV_ENGINE_ERROR("Image::LoadFromMemory() - {:s} 형식의 파일 로드에 실패하였습니다.", extension);
			return false;
		}

		loadScrachImage(img);
		img.Release();

		return true;
	}

	void Image::loadScrachImage(const DirectX::ScratchImage& img)
	{
		const auto& metaData = img.GetMetadata();
		m_Width = static_cast<int>(metaData.width);
		m_Height = static_cast<int>(metaData.height);
		m_Depth = static_cast<int>(metaData.depth);
		m_Format = metaData.format;
		m_bCubemap = metaData.IsCubemap();

		// 텍스쳐가 배열이거나 밉맵을 가질 수 있다.
		// 그렇다면 개별 Image로 접근해야 할 것 같다.
		auto pPixels = img.GetImages()->pixels;
		auto slicePitch = img.GetImages()->slicePitch;

		// 일단 그려진다. 하지만 크기에 확신이 없다.
		m_Pixels.resize(slicePitch);
		for (size_t i = 0; i != slicePitch; ++i)
			m_Pixels[i] = pPixels[i];

		m_bOpaque = img.IsAlphaAllOpaque();
	}
}