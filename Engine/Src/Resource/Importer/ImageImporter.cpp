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

	// 스파르탄처럼 sliceIndex를 매개변수로 추가하는 편이 나을 것 같다.
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
			DV_CORE_ERROR("{:s} 로드에 실패하였습니다.", filepath);
			return false;
		}

		const auto pImages = baseImage.GetImages();
		const auto& texMetadata = baseImage.GetMetadata();

		auto width = texMetadata.width;
		auto height = texMetadata.height;
		auto arraySize = texMetadata.arraySize;
		auto mipLevels = texMetadata.mipLevels;
		auto format = texMetadata.format;
		// cubemap일 경우 배열별로 받아야 한다...?
		auto rowPitch = pImages->rowPitch;
		auto slicePitch = pImages->slicePitch;

		// data를 저장을 어떻게 해야할 지 생각해봐야 한다.
		// array를 만들거면 6개 이상일 수 있다.
		// 그리고 mipmap도 생각해야 한다. => 밉맵을 이 곳에서 생성한다면 좀 간단해질듯?
		auto pPixels = pImages->pixels;	// 복사해야 한다. 아니면 지워질듯...

		// 이걸로 밉맵을 생성한다.
		// 추가 텍스쳐를 만들어야 해서 mipChain으로 받는 듯 하다.
		DirectX::ScratchImage mipChain;
		DirectX::GenerateMipMaps(baseImage.GetImages(), baseImage.GetImageCount(), baseImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

		// 0번째가 원본과 동일하고, 1x1까지 생성하는 듯 하다.
		auto count = mipChain.GetImageCount();

		baseImage.Release();

		// texture에 대입
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

		// 내부에서 만드는 것은 에바다.
		// flags와 mipmap 생성 여부 등을 추가로 설정할 수 있어야 한다.
		// 스파르탄은 생성자에서 이 값들을 매개변수로 전달한 후 임포터에서 Load한다.
		// 정확하게 정리하자면
		// 1. 텍스쳐를 생성(flags, generageMips)
		// 2. 생성한 텍스쳐의 LoadFromFile에 파일 경로를 전달(slice index 필요)
		// 3. 내부에서 임포터로 Load(slice index에 맞게 data도 저장)
		// 4. 밉맵 생성 여부 처리 후 Create(rtv, dsv의 경우 array 생성이 필요할 수 있음)

		return true;
	}
}