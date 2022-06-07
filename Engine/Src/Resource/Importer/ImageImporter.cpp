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

	// ���ĸ�źó�� sliceIndex�� �Ű������� �߰��ϴ� ���� ���� �� ����.
	bool ImageImporter::Load(const std::string& filepath, Texture* pTexture)
	{
		if (!pTexture)
			return false;

		DirectX::ScratchImage baseImage;

		std::wstring tempPath(filepath.begin(), filepath.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

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
			DV_CORE_ERROR("{:s} �ε忡 �����Ͽ����ϴ�.", filepath);
			return false;
		}

		const auto pImages = baseImage.GetImages();
		const auto& texMetadata = baseImage.GetMetadata();

		auto width = texMetadata.width;
		auto height = texMetadata.height;
		auto arraySize = texMetadata.arraySize;
		auto mipLevels = texMetadata.mipLevels;
		auto format = texMetadata.format;
		// cubemap�� ��� �迭���� �޾ƾ� �Ѵ�...?
		auto rowPitch = pImages->rowPitch;
		auto slicePitch = pImages->slicePitch;

		// data�� ������ ��� �ؾ��� �� �����غ��� �Ѵ�.
		// array�� ����Ÿ� 6�� �̻��� �� �ִ�.
		// �׸��� mipmap�� �����ؾ� �Ѵ�. => �Ӹ��� �� ������ �����Ѵٸ� �� ����������?
		auto pPixels = pImages->pixels;	// �����ؾ� �Ѵ�. �ƴϸ� ��������...

		// �̰ɷ� �Ӹ��� �����Ѵ�.
		// �߰� �ؽ��ĸ� ������ �ؼ� mipChain���� �޴� �� �ϴ�.
		DirectX::ScratchImage mipChain;
		DirectX::GenerateMipMaps(baseImage.GetImages(), baseImage.GetImageCount(), baseImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

		// 0��°�� ������ �����ϰ�, 1x1���� �����ϴ� �� �ϴ�.
		auto count = mipChain.GetImageCount();

		baseImage.Release();

		// texture�� ����
		pTexture->SetWidth((const uint32_t)width);
		pTexture->SetHeight((const uint32_t)height);
		pTexture->SetArraySize((const uint32_t)arraySize);
		pTexture->SetMipLevels((const uint32_t)mipLevels);
		pTexture->SetFormat((const DXGI_FORMAT)format);
		pTexture->SetRowPitch((const uint32_t)rowPitch);
		pTexture->SetSlicePitch((const uint32_t)slicePitch);
		// channel count?
		// sub resource = pixels?
		pPixels;

		// ���ο��� ����� ���� ���ٴ�.
		// flags�� mipmap ���� ���� ���� �߰��� ������ �� �־�� �Ѵ�.
		// ���ĸ�ź�� �����ڿ��� �� ������ �Ű������� ������ �� �����Ϳ��� Load�Ѵ�.
		// ��Ȯ�ϰ� �������ڸ�
		// 1. �ؽ��ĸ� ����(flags, generageMips)
		// 2. ������ �ؽ����� LoadFromFile�� ���� ��θ� ����(slice index �ʿ�)
		// 3. ���ο��� �����ͷ� Load(slice index�� �°� data�� ����)
		// 4. �Ӹ� ���� ���� ó�� �� Create(rtv, dsv�� ��� array ������ �ʿ��� �� ����)

		return true;
	}
}