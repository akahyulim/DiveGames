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
			// �ᱹ �̱����� ���� �� ��︰��.
			ImageImporter() = default;
			~ImageImporter() = default;

			dive::dvTexture2D* LoadFile(const std::wstring& filepath, bool generateMips = false);

		private:

		};
	}
}