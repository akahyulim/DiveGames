#include "ImageImporter.h"

namespace Editor
{
	namespace Importer
	{
		dive::dvTexture2D* ImageImporter::LoadFile(const std::wstring& filepath, bool generateMips)
		{
			// ���� Ȯ��

			DirectX::ScratchImage image;
			if (FAILED(DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image)))
			{
				APP_ERROR("");
				return nullptr;
			}

			// Mipmap, Array�� Ȯ���� �� ������...
			auto data = image.GetMetadata();

			auto pNewTex = new dive::dvTexture2D(data.width, data.height, data.format, generateMips);
			if (!pNewTex->LoadData(image.GetPixels()))
			{
				APP_ERROR("");
				DV_DELETE(pNewTex);
				return nullptr;
			}

			return pNewTex;
		}
	}
}