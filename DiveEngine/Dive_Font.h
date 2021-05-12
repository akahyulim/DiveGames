#pragma once
#include "Texture.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

// 문제점
// 1. 문자의 경우 상하 위치가 안맞는다.
// 2. 한글의 문자수가 너무 많아 전부 ATLAS로 만들면 크기가 너무 크다.
// 실제로 Font Size를 조절하지 않으면 생성이 불가능하다. + 영문자와 특수기호도 생각해야 한다.
// 3. 자음 혹은 모음만으로 출력이 불가능하다. 그렇다면 현재 적용한 유니코드는 완성형이라는 건가?
// 채팅 프로그램을 만들려면 조합형으로 할텐데...

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

		Dive_Glyph GetGlyph(unsigned int key);

	private:
		void printChar(BYTE* pTexels, FT_Face& face, DirectX::XMUINT2& pen, FT_ULong ch);

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