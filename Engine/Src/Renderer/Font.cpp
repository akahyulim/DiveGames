#include "Font.h"
#include "Graphics/GraphicsInclude.h"
#include "Renderer.h"

namespace DiveEngine
{
	static const unsigned int GLYPH_BASIC_LATIN_START = 0x0000;
	static const unsigned int GLYPH_BASIC_LATIN_END = 0x007F;

	static const unsigned int GLYPH_KOREAN_START =  0xAC00;
	static const unsigned int GLYPH_KOREAN_END = 0xD7B0;

	static const unsigned int ATLAS_WIDTH	= 1024;

	// 비 멤버, 비 프렌드 함수

	//  glyph에서 최대 큰 가로, 세로 크기를 가져온다.
	inline void GetCharacterMaxDimension(const FT_Face& face, unsigned int& maxWidth, unsigned int& maxHeight, unsigned int codeStart, unsigned int codeEnd)
	{
		unsigned int width	= 0;
		unsigned int height = 0;

		for (unsigned int charCode = codeStart; charCode != codeEnd; charCode++)
		{
			if (0 != FT_Load_Char(face, charCode, 0))
				continue;

			auto pBitmap	= &face->glyph->bitmap;
			width			= max(width, pBitmap->width);
			height			= max(height, pBitmap->rows);
		}

		maxWidth	= width;
		maxHeight	= height;
	}

	// Glyph들의 크기와 Atlas의 크기 계산
	inline void GetAtlasDeimension(const FT_Face& face, unsigned int& atlasWidth, unsigned int& atlasHeight,
		unsigned int& atlasCellWidth, unsigned int& atlasCellHeight, unsigned int codeStart, unsigned int codeEnd)
	{
		unsigned int maxWidth	= 0;
		unsigned int maxHeight	= 0;

		GetCharacterMaxDimension(face, maxWidth, maxHeight, codeStart, codeEnd);

		const unsigned int glyphCount	= codeEnd - codeStart;
		const unsigned int glyphPerRow	= ATLAS_WIDTH / maxWidth;
		const unsigned int rowCount		= static_cast<unsigned int>(ceil(static_cast<float>(glyphCount) / static_cast<float>(glyphPerRow)));

		atlasWidth		= ATLAS_WIDTH;
		atlasHeight		= maxHeight * rowCount;
		atlasCellWidth	= maxWidth;
		atlasCellHeight = maxHeight;
	}

	Font::Font()
	{
		m_fontSize = 20;
		FT_Init_FreeType(&m_libFt);
	}

	Font::~Font()
	{
		FT_Done_FreeType(m_libFt);
	}

	// 크기는 디폴트, 아니라면 미리 SetFontSize???
	bool Font::LoadFromFile(const std::string& filepath, FT_Long faceIndex)
	{
		// 현재 파일 설정과 같다면 재생성하지 않는다? 크기가 다르다면?

		// 파일이 존재한다?

		// 이미 만들어져 있다면 다시 생성
		// 즉, clear 필요?

		// 1. 우선 기본 문자셋부터 만든다.
		// 해당 폰트에 기본 문자셋이 없다면 디폴트 문자를 사용한다.

		FT_Face face;
		FT_New_Face(m_libFt, filepath.c_str(), faceIndex, &face);
		FT_Set_Char_Size(face, m_fontSize * 64, 0, 96, 0);
		GetAtlasDeimension(face, m_atlasWidth, m_atlasHeight, m_atlasCellWidth, m_atlasCellHeight, GLYPH_BASIC_LATIN_START, GLYPH_BASIC_LATIN_END);

		m_pAtlas = new Texture(m_atlasWidth, m_atlasHeight);	

		auto pImmediateContex = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pImmediateContex->Map(m_pAtlas->GetTexture2D(),
			D3D11CalcSubresource(0, 0, 1),
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource);

		BYTE* pTexels = (BYTE*)mappedResource.pData;

		DirectX::XMUINT2 pen = DirectX::XMUINT2(0, 0);

		for (unsigned int charCode = GLYPH_BASIC_LATIN_START; charCode != GLYPH_BASIC_LATIN_END; charCode++)
		{
			printChar(pTexels, face, pen, charCode);
		}
		
		pImmediateContex->Unmap(m_pAtlas->GetTexture2D(), D3D11CalcSubresource(0, 0, 1));

		FT_Done_Face(face);

		return true;
	}

	void Font::SetFontSize(unsigned int size)
	{
		// 너무 자유분방하게 생각하고 있다.
		// Atlas가 생성되어 있어야 크기도 변경할 수 있다고 여기는 편이 낫다.

		if (m_fontSize != size)
		{
			// 이미 Atals가 구성되어 있다면 다시 생성
			// Atlas로 구분할 수 없다면 Glyphs로 판단해야 한다.

			m_fontSize = size;
		}
	}

	Glyph Font::GetGlyph(unsigned int key)
	{
		assert(!m_glyphs.empty()); 
		return m_glyphs[key];
	}

	void Font::printChar(BYTE* pTexels, FT_Face& face, DirectX::XMUINT2& pen, FT_ULong ch)
	{
		// RowPitch는 결국 AtlasWidth와 같다.

		unsigned index = FT_Get_Char_Index(face, ch);
		FT_Load_Glyph(face, index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);

		// 함수로 보기 좋게 묶기
		m_glyphs[ch].offsetX = face->glyph->metrics.horiBearingX >> 6;
		m_glyphs[ch].offsetY = face->glyph->metrics.horiBearingY >> 6;
		//m_glyphs[ch].horizontalAdvence = face->glyph->metrics.horiAdvance >> 6;
		m_glyphs[ch].width		= face->glyph->metrics.width >> 6;
		m_glyphs[ch].height		= face->glyph->metrics.height >> 6;
		m_glyphs[ch].uvLeft		= (float)pen.x / (float)m_atlasWidth;
		m_glyphs[ch].uvRight	= ((float)pen.x + (float)m_glyphs[ch].width) / (float)m_atlasWidth;
		m_glyphs[ch].uvTop		= (float)pen.y / (float)m_atlasHeight;
		m_glyphs[ch].uvBottom	= ((float)pen.y + (float)m_glyphs[ch].height) / (float)m_atlasHeight;

		int width = face->glyph->bitmap.width;
		int height = face->glyph->bitmap.rows;

		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				BYTE value = face->glyph->bitmap.buffer[col + row * width];

				int atlasX = pen.x + col;// +face->glyph->bitmap_left;
				int atlasY = pen.y + row;// +face->glyph->bitmap_top;

				pTexels[atlasX + atlasY * m_atlasWidth] = value;
			}
		}

		pen.x += m_atlasCellWidth;

		if (pen.x + m_atlasCellWidth > m_atlasWidth)
		{
			pen.x = 0;
			pen.y += m_atlasCellHeight;
		}
	}
}