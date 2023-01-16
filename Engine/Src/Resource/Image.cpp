#include "divepch.h"
#include "Image.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	Image::Image(Context* pContext)
		: Resource(pContext),
		m_Width(0),
		m_Height(0),
		m_Depth(0),
		m_Format(DXGI_FORMAT_R8G8B8A8_UNORM),
		m_bCubemap(false)
	{
	}

	Image::~Image()
	{

	}

	bool Image::LoadFromFile(const std::string& fileName)
	{
		auto filePath = FileSystem::StringToWstring(fileName);
		auto ext = FileSystem::GetExtension(fileName);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		if (ext == ".tga")
		{
			hResult = DirectX::LoadFromTGAFile(filePath.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_LOG_ENGINE_ERROR("Image::LoadFromFile - {:s} 로드에 실패하였습니다.", fileName);
			return false;
		}

		const auto& metaData = img.GetMetadata();
		m_Width = static_cast<int>(metaData.width);
		m_Height = static_cast<int>(metaData.height);
		m_Depth = static_cast<int>(metaData.depth);
		m_Format = metaData.format;
		m_bCubemap =  metaData.IsCubemap();

		// 텍스쳐가 배열이거나 밉맵을 가질 수 있다.
		// 그렇다면 개별 Image로 접근해야 할 것 같다.
		auto pPixels = img.GetImages()->pixels;
		auto slicePitch = img.GetImages()->slicePitch;
		
		// 일단 그려진다. 하지만 크기에 확신이 없다.
		m_Pixels.resize(slicePitch);
		for (size_t i = 0; i != slicePitch; ++i)
			m_Pixels[i] = pPixels[i];

		img.Release();

		return true;
	}
}