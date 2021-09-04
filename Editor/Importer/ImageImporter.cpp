#include "ImageImporter.h"

namespace Editor
{
	// Importer들끼리 묶고 싶어서 만든건데
	// 좀 에반가...?
	namespace Importer
	{
		// 비 멤버 함수
		unsigned int GetBytesPerChannel(FIBITMAP* pBitmap)
		{
			if (pBitmap == nullptr)
			{
				APP_ERROR("");
				return 0;
			}

			auto type = FreeImage_GetImageType(pBitmap);
			unsigned int size = 0;

			if (type == FIT_BITMAP)
			{
				size = sizeof(BYTE);
			}
			else if (type == FIT_UINT16 || type == FIT_RGB16 || type == FIT_RGBA16)
			{
				size = sizeof(WORD);
			}
			else if (type == FIT_FLOAT || type == FIT_RGBF || type == FIT_RGBAF)
			{
				size = sizeof(float);
			}

			return size;
		}

		unsigned int GetChannelCount(FIBITMAP* pBitmap)
		{
			if (pBitmap == nullptr)
			{
				APP_ERROR("");
				return 0;
			}

			auto bytesPerPixel = FreeImage_GetLine(pBitmap) / FreeImage_GetWidth(pBitmap);
			auto channelCount = bytesPerPixel / GetBytesPerChannel(pBitmap);

			return channelCount;
		}

		DXGI_FORMAT GetDxgiFormat(unsigned int bytesPerChannel, unsigned int channelCount)
		{
			auto bitsPerChannel = bytesPerChannel * 8;

			auto format = DXGI_FORMAT_UNKNOWN;

			if (channelCount == 1)
			{
				if (bitsPerChannel == 8)	format = DXGI_FORMAT_R8_UNORM;
			}
			else if (channelCount == 2)
			{
				if (bitsPerChannel == 8)	format = DXGI_FORMAT_R8G8_UNORM;
			}
			else if (channelCount == 3)
			{
				if (bitsPerChannel == 8)	format = DXGI_FORMAT_R8G8_B8G8_UNORM;
				if (bitsPerChannel == 32)	format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (channelCount == 4)
			{
				if (bitsPerChannel == 8)	format = DXGI_FORMAT_R8G8B8A8_UNORM;
				if (bitsPerChannel == 16)	format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				if (bitsPerChannel == 32)	format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			return format;
		}

		ImageImporter::ImageImporter()
		{
			FreeImage_Initialise();
		}

		ImageImporter::~ImageImporter()
		{
			FreeImage_DeInitialise();
		}

		// 스파르탄에서 이걸 호출하는 부분은 Texture::LoadFromFile()이다.
		// 즉, 생성된 Texture가 Importer를 이용해 자신의 포인터를 넘겨 데이터를 얻어오는 것이다.
		// 따라서 내부에서 기존 Resource Release -> Load -> Resource 다시 생성의 과정을 거치는 것이다.
		// 이는 Importer를 Editor에서 구현한 내 디자인과는 맞지 않다.
		// 1. 내부에서 Texture를 직접 생성하는 건 에바다.
		// => ResourceManager가 생성해야 하다.
		// => 그러고보니 스파르탄의 RenderTarget는 ResourceManager가 생성하지 않았다....
		// => 대충 찾아보니 RenderTarget은 그냥 막생성 및 관리이고, Material을 비롯한 리소스들은 매니져를 이용하는 것 같다.
		// => Material을 예로 들어보자면 ResourceCache의 Load에서 Texture를 생성한 후 Texture::LoadFromFile()에 경로를 전달하여 생성한다.
		// 2. 외부에서 생성된 Texture를 받아서 구성하려면 이에 걸맞는 멤버 함수들이 필요하다.
		// => 유니티 인터페이스를 따르자면 Resize() 후 Laod()가 맞다.
		// => 그렇다면 size, format, generateMips와 Data만 전달하면 된다.
		// => 그런데 Resize()라는 이름은 전혀 어울리지 않는다. 
		bool ImageImporter::Load(const std::string& filepath, dive::dvTexture2D* pTexture, bool generateMimmaps)
		{
			// 파일 확인

			// pTexture 확인
			

			// Load
			auto format = FreeImage_GetFileType(filepath.c_str(), 0);
			format = (format == FIF_UNKNOWN) ? FreeImage_GetFIFFromFilename(filepath.c_str()) : format;
			if (!FreeImage_FIFSupportsReading(format))
			{
				APP_ERROR("지원하지 않는 포멧");
				return false;
			}

			auto pBitmap = FreeImage_Load(format, filepath.c_str());
			if (!pBitmap)
			{
				APP_ERROR("Load 실패");
				return false;
			}

			// 일단 const로 정보를 얻어온다.
			const unsigned int imageWidth = FreeImage_GetWidth(pBitmap);
			const unsigned int imageHeight = FreeImage_GetHeight(pBitmap);
			const DXGI_FORMAT imageFormat = GetDxgiFormat(GetBytesPerChannel(pBitmap), GetChannelCount(pBitmap));
			BYTE* pByte = FreeImage_GetBits(pBitmap);	// 이건 바로 전달해야 한다.

			FreeImage_Unload(pBitmap);

			// pTexture에 Data 전달

			return true;
		}
	}
}