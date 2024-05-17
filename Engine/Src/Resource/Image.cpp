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
			DV_ENGINE_ERROR("Image::LoadFromFile - {:s} �ε忡 �����Ͽ����ϴ�.", fileName);
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
			DV_ENGINE_ERROR("Image::LoadFromMemory() - {:s} ������ ���� �ε忡 �����Ͽ����ϴ�.", extension);
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

		// �ؽ��İ� �迭�̰ų� �Ӹ��� ���� �� �ִ�.
		// �׷��ٸ� ���� Image�� �����ؾ� �� �� ����.
		auto pPixels = img.GetImages()->pixels;
		auto slicePitch = img.GetImages()->slicePitch;

		// �ϴ� �׷�����. ������ ũ�⿡ Ȯ���� ����.
		m_Pixels.resize(slicePitch);
		for (size_t i = 0; i != slicePitch; ++i)
			m_Pixels[i] = pPixels[i];

		m_bOpaque = img.IsAlphaAllOpaque();
	}
}