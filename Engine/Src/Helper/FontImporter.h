#pragma once
#include "../Renderer/Font.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <string>

namespace DiveEngine
{
	class FontImporter
	{
	public:
		FontImporter();
		~FontImporter();

		bool ImporterFile(Font* pFont, const std::string& filepath);

	private:
	private:
		FT_LibraryRec_* m_pLibrary = nullptr;
	};
}