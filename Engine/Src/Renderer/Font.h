#pragma once
#include "Graphics/Texture.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

// 문제점
// 1. 영문자와 한글을 구분할 것인가?
// => 그냥 해당 폰트 파일에 적재된 모든 문자를 만들면 안되나?
// 2. Outline을 적용할 것인가?

namespace DiveEngine
{
	struct Glyph
	{
		unsigned int offsetX = 0;
		unsigned int offsetY = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int horizontalAdvence = 0;

		float uvLeft = 0.0f;
		float uvRight = 0.0f;
		float uvTop = 0.0f;
		float uvBottom = 0.0f;
	};

	// 일단 임포터 없이 만들자.
	class Font
	{
	public:
		Font();
		~Font();

		// 일단 유니코드만 지원
		bool LoadFromFile(const std::string& filepath, FT_Long faceIndex = 0);

		void SetFontSize(unsigned int size);
		unsigned int GetFontSize() const { return m_fontSize; }

		Texture* GetAtlas() { return m_pAtlas; }
		Glyph GetGlyph(unsigned int key);

	private:
		void printChar(BYTE* pTexels, FT_Face& face, DirectX::XMUINT2& pen, FT_ULong ch);

	private:
		FT_Library m_libFt;

		Texture* m_pAtlas = nullptr;

		unsigned int m_atlasWidth = 0;
		unsigned int m_atlasHeight = 0;

		unsigned int m_atlasCellWidth = 0;
		unsigned int m_atlasCellHeight = 0;

		std::unordered_map<unsigned int, Glyph> m_glyphs;

		unsigned int m_fontSize;
	};
}