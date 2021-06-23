#pragma once
#include "Font.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H

namespace dive
{
	class FontImporter
	{
	public:
		FontImporter();
		~FontImporter();

		bool ImporterFile(Font* pFont, const std::string& filepath);

	private:
	private:
		FT_LibraryRec_* m_library = nullptr;
	};
}