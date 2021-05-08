#pragma once
#include "Texture.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

namespace Dive
{
	struct Dive_Glyph
	{
		unsigned int offsetX = 0;
		unsigned int offsetY = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int horizontalAdvence = 0;		// 이건 뭘까?

		float uvLeft = 0.0f;
		float uvRight = 0.0f;
		float uvTop = 0.0f;
		float uvBottom = 0.0f;
	};

	// 일단 임포터 없이 만들자.
	class Dive_Font
	{
	public:
		Dive_Font();
		~Dive_Font();

		bool LoadFromFile(const std::string& filepath, FT_Long faceIndex = 0);

		void SetFontSize(unsigned int size);

		Texture* GetAtlas() { return m_pAtlas; }

	private:
		void printChar(BYTE* pTexels, unsigned int rowPitch, FT_Face& face, DirectX::XMFLOAT2& pen, FT_ULong ch);

	private:
		FT_Library m_libFt;

		Texture* m_pAtlas = nullptr;

		unsigned int m_atlasWidth = 0;
		unsigned int m_atlasHeight = 0;

		unsigned int m_atlasCellWidth = 0;
		unsigned int m_atlasCellHeight = 0;

		// 문자 코드와 glyph 조합이다. 근데 문자 코드는 unicode가 좋을려나?
		std::unordered_map<unsigned int, Dive_Glyph> m_glyphs;

		unsigned int m_fontSize;
	};
}