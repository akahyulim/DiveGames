#include "ImageImporter.h"

namespace Editor
{
	// Importer�鳢�� ���� �; ����ǵ�
	// �� ���ݰ�...?
	namespace Importer
	{
		// �� ��� �Լ�
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

		// ���ĸ�ź���� �̰� ȣ���ϴ� �κ��� Texture::LoadFromFile()�̴�.
		// ��, ������ Texture�� Importer�� �̿��� �ڽ��� �����͸� �Ѱ� �����͸� ������ ���̴�.
		// ���� ���ο��� ���� Resource Release -> Load -> Resource �ٽ� ������ ������ ��ġ�� ���̴�.
		// �̴� Importer�� Editor���� ������ �� �����ΰ��� ���� �ʴ�.
		// 1. ���ο��� Texture�� ���� �����ϴ� �� ���ٴ�.
		// => ResourceManager�� �����ؾ� �ϴ�.
		// => �׷����� ���ĸ�ź�� RenderTarget�� ResourceManager�� �������� �ʾҴ�....
		// => ���� ã�ƺ��� RenderTarget�� �׳� ������ �� �����̰�, Material�� ����� ���ҽ����� �Ŵ����� �̿��ϴ� �� ����.
		// => Material�� ���� ���ڸ� ResourceCache�� Load���� Texture�� ������ �� Texture::LoadFromFile()�� ��θ� �����Ͽ� �����Ѵ�.
		// 2. �ܺο��� ������ Texture�� �޾Ƽ� �����Ϸ��� �̿� �ɸ´� ��� �Լ����� �ʿ��ϴ�.
		// => ����Ƽ �������̽��� �����ڸ� Resize() �� Laod()�� �´�.
		// => �׷��ٸ� size, format, generateMips�� Data�� �����ϸ� �ȴ�.
		// => �׷��� Resize()��� �̸��� ���� ��︮�� �ʴ´�. 
		bool ImageImporter::Load(const std::string& filepath, dive::dvTexture2D* pTexture, bool generateMimmaps)
		{
			// ���� Ȯ��

			// pTexture Ȯ��
			

			// Load
			auto format = FreeImage_GetFileType(filepath.c_str(), 0);
			format = (format == FIF_UNKNOWN) ? FreeImage_GetFIFFromFilename(filepath.c_str()) : format;
			if (!FreeImage_FIFSupportsReading(format))
			{
				APP_ERROR("�������� �ʴ� ����");
				return false;
			}

			auto pBitmap = FreeImage_Load(format, filepath.c_str());
			if (!pBitmap)
			{
				APP_ERROR("Load ����");
				return false;
			}

			// �ϴ� const�� ������ ���´�.
			const unsigned int imageWidth = FreeImage_GetWidth(pBitmap);
			const unsigned int imageHeight = FreeImage_GetHeight(pBitmap);
			const DXGI_FORMAT imageFormat = GetDxgiFormat(GetBytesPerChannel(pBitmap), GetChannelCount(pBitmap));
			BYTE* pByte = FreeImage_GetBits(pBitmap);	// �̰� �ٷ� �����ؾ� �Ѵ�.

			FreeImage_Unload(pBitmap);

			// pTexture�� Data ����

			return true;
		}
	}
}