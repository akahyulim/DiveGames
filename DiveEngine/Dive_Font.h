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
		unsigned int horizontalAdvence = 0;		// �̰� ����?

		float uvLeft = 0.0f;
		float uvRight = 0.0f;
		float uvTop = 0.0f;
		float uvBottom = 0.0f;
	};

	// �ϴ� ������ ���� ������.
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

		// ���� �ڵ�� glyph �����̴�. �ٵ� ���� �ڵ�� unicode�� ��������?
		std::unordered_map<unsigned int, Dive_Glyph> m_glyphs;

		unsigned int m_fontSize;
	};
}