#pragma once
#include "DiveEngine.h"
#include "FreeImage/FreeImage.h"

namespace Editor
{
	namespace Importer
	{
		class ImageImporter
		{
		public:
			// 결국 싱글톤이 가장 잘 어울린다.
			ImageImporter() = default;
			~ImageImporter() = default;

			dive::dvTexture2D* LoadFile(const std::wstring& filepath, bool generateMips = false);

		private:

		};
	}
}