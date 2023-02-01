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
			DV_LOG_ENGINE_ERROR("Image::LoadFromFile - {:s} �ε忡 �����Ͽ����ϴ�.", fileName);
			return false;
		}

		const auto& metaData = img.GetMetadata();
		m_Width = static_cast<int>(metaData.width);
		m_Height = static_cast<int>(metaData.height);
		m_Depth = static_cast<int>(metaData.depth);
		m_Format = metaData.format;
		m_bCubemap =  metaData.IsCubemap();

		// �ؽ��İ� �迭�̰ų� �Ӹ��� ���� �� �ִ�.
		// �׷��ٸ� ���� Image�� �����ؾ� �� �� ����.
		auto pPixels = img.GetImages()->pixels;
		auto slicePitch = img.GetImages()->slicePitch;
		
		// �ϴ� �׷�����. ������ ũ�⿡ Ȯ���� ����.
		m_Pixels.resize(slicePitch);
		for (size_t i = 0; i != slicePitch; ++i)
			m_Pixels[i] = pPixels[i];

		img.Release();

		return true;
	}
}