#include "ImageImporter.h"

namespace Editor
{
	// Importer들끼리 묶고 싶어서 만든건데
	// 좀 에반가...?
	namespace Importer
	{
		ImageImporter::ImageImporter()
		{
			// FreeImage Initialize
		}

		ImageImporter::~ImageImporter()
		{
			// FreeImage DeInitialized
		}

		bool ImageImporter::Load(const std::string& filepath, dive::dvTexture2D* pTexture, bool generateMimmaps)
		{
			// 파일 확인

			// pTexture 확인

			// Load

			// pTexture에 Data 전달

			return true;
		}
	}
}