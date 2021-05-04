#pragma once
#include "Dive_Font.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H

namespace Dive
{
	class FontImporter
	{
	public:
		FontImporter();
		~FontImporter();

		bool ImporterFile(Dive_Font* pFont, const std::string& filepath);

	private:
	private:
		FT_LibraryRec_* m_library = nullptr;
	};
}