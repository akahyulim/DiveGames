#pragma once
#include "Renderer.h"
#include "Texture.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H
#include <string>

namespace Dive
{
	struct Dive_Glyph
	{

	};

	// 일단 임포터 없이 만들자.
	class Dive_Font
	{
	public:
		Dive_Font();
		~Dive_Font();

		bool LoadFromFile(const std::string& filepath, FT_Long faceIndex = 0);

	private:
	private:
		FT_Library m_libFt;
		FT_Face m_face;

		Texture* m_pAtlas;
		Texture* m_pAtlasOutline;
	};
}