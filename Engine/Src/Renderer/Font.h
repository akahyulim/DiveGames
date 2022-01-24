#pragma once
#include "Graphics/Texture.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

// ������
// 1. �����ڿ� �ѱ��� ������ ���ΰ�?
// => �׳� �ش� ��Ʈ ���Ͽ� ����� ��� ���ڸ� ����� �ȵǳ�?
// 2. Outline�� ������ ���ΰ�?

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

	// �ϴ� ������ ���� ������.
	class Font
	{
	public:
		Font();
		~Font();

		// �ϴ� �����ڵ常 ����
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