#pragma once
#include "DiveEngine.h"

namespace Editor
{
	namespace Importer
	{
		class ImageImporter
		{
		public:
			ImageImporter();
			~ImageImporter();

			bool Load(const std::string& filepath, dive::dvTexture2D* pTexture, bool generateMimmaps = false);

		private:

		};
	}
}