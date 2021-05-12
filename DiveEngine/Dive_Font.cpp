#include "Dive_Font.h"
#include "GraphicsInclude.h"
#include "Renderer.h"

namespace Dive
{
	// ���� 11,184����. �ݸ� ks1001�� 2350����.
	// �׷��� ks1001�� �����ڵ�� �ٸ���.
	static const unsigned int GLYPH_START = 0xAC00; //32;
	static const unsigned int GLYPH_END = 0xD7B0; // 127;
	static const unsigned int ATLAS_WIDTH	= 1024;

	// �� ���, �� ������ �Լ�

	//  glyph���� �ִ� ū ����, ���� ũ�⸦ �����´�.
	inline void GetCharacterMaxDimension(const FT_Face& face, unsigned int& maxWidth, unsigned int& maxHeight)
	{
		unsigned int width	= 0;
		unsigned int height = 0;

		for (unsigned int charCode = GLYPH_START; charCode != GLYPH_END; charCode++)
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

	// Glyph���� ũ��� Atlas�� ũ�� ���
	inline void GetAtlasDeimension(const FT_Face& face, unsigned int& atlasWidth, unsigned int& atlasHeight,
		unsigned int& atlasCellWidth, unsigned int& atlasCellHeight)
	{
		unsigned int maxWidth	= 0;
		unsigned int maxHeight	= 0;

		GetCharacterMaxDimension(face, maxWidth, maxHeight);

		// Atlas�� Width�� ����� �����س��Ҵ�. = 512
		// Height�� ����� �ʿ��ϴ�.
		const unsigned int glyphCount	= GLYPH_END - GLYPH_START;
		const unsigned int glyphPerRow	= ATLAS_WIDTH / maxWidth;
		const unsigned int rowCount		= static_cast<unsigned int>(ceil(static_cast<float>(glyphCount) / static_cast<float>(glyphPerRow)));

		atlasWidth		= ATLAS_WIDTH;
		atlasHeight		= maxHeight * rowCount;
		atlasCellWidth	= maxWidth;
		atlasCellHeight = maxHeight;
	}

	Dive_Font::Dive_Font()
	{
		FT_Init_FreeType(&m_libFt);
	}

	Dive_Font::~Dive_Font()
	{
		FT_Done_FreeType(m_libFt);
	}

	// ũ��� ����Ʈ, �ƴ϶�� �̸� SetFontSize???
	bool Dive_Font::LoadFromFile(const std::string& filepath, FT_Long faceIndex)
	{
		// ���� ���� ������ ���ٸ� ��������� �ʴ´�? ũ�Ⱑ �ٸ��ٸ�?

		// ������ �����Ѵ�?

		// �̹� ������� �ִٸ� �ٽ� ����
		// ��, clear �ʿ�?

		FT_Face face;
		FT_New_Face(m_libFt, filepath.c_str(), faceIndex, &face);

		// ũ��� �̸� �����ϴ� �� �´�.
		FT_Set_Char_Size(face, 14 * 64, 0, 96, 0);

		GetAtlasDeimension(face, m_atlasWidth, m_atlasHeight, m_atlasCellWidth, m_atlasCellHeight);

		m_pAtlas = new Texture(m_atlasWidth, m_atlasHeight);	

		auto pImmediateContex = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pImmediateContex->Map(m_pAtlas->GetTexture2D(),
			D3D11CalcSubresource(0, 0, 1),
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource);

		//UCHAR* pTexels = (UCHAR*)mappedResource.pData;
		BYTE* pTexels = (BYTE*)mappedResource.pData;

		// �̰� �ٽ� Float2�� �ٲ�� �Ѵ�.
		DirectX::XMUINT2 pen = DirectX::XMUINT2(0, 0);
		// ���ĺ� + Ư�� ����: �̰� �����Ϸ��� ũ����� ���� �����ؾ� �Ѵ�.
		/*
		for (unsigned int charCode = 32; charCode != 127; charCode++)
		{
			printChar(pTexels, face, pen, charCode);

			pen.x += m_atlasCellWidth;

			if (pen.x + m_atlasCellWidth > m_atlasWidth)
			{
				pen.x = 0;
				pen.y += m_atlasCellHeight;
			}
		}
		*/
		// local ����
		for (unsigned int charCode = GLYPH_START; charCode != GLYPH_END; charCode++)
		{
			printChar(pTexels, face, pen, charCode);

			pen.x += m_atlasCellWidth;

			if (pen.x + m_atlasCellWidth > m_atlasWidth)
			{
				pen.x = 0;
				pen.y += m_atlasCellHeight;
			}
		}
		
		pImmediateContex->Unmap(m_pAtlas->GetTexture2D(), D3D11CalcSubresource(0, 0, 1));

		FT_Done_Face(face);

		return true;
	}

	void Dive_Font::SetFontSize(unsigned int size)
	{
		// �ʹ� �����й��ϰ� �����ϰ� �ִ�.
		// Atlas�� �����Ǿ� �־�� ũ�⵵ ������ �� �ִٰ� ����� ���� ����.

		if (m_fontSize != size)
		{
			// �̹� Atals�� �����Ǿ� �ִٸ� �ٽ� ����
			// Atlas�� ������ �� ���ٸ� Glyphs�� �Ǵ��ؾ� �Ѵ�.

			m_fontSize = size;
		}
	}

	Dive_Glyph Dive_Font::GetGlyph(unsigned int key)
	{
		assert(!m_glyphs.empty()); 
		return m_glyphs[key];
	}

	void Dive_Font::printChar(BYTE* pTexels, FT_Face& face, DirectX::XMUINT2& pen, FT_ULong ch)
	{
		// RowPitch�� �ᱹ AtlasWidth�� ����.

		unsigned index = FT_Get_Char_Index(face, ch);
		FT_Load_Glyph(face, index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);

		// �Լ��� ���� ���� ����
		//m_glyphs[ch].offsetX = face->glyph->metrics.horiBearingX >> 6;
		//m_glyphs[ch].offsetY = face->glyph->metrics.horiBearingY >> 6;
		//m_glyphs[ch].horizontalAdvence = face->glyph->metrics.horiAdvance >> 6;
		m_glyphs[ch].width		= face->glyph->metrics.width >> 6;
		m_glyphs[ch].height		= face->glyph->metrics.height >> 6;
		m_glyphs[ch].uvLeft		= (float)pen.x / (float)m_atlasWidth;
		m_glyphs[ch].uvRight	= ((float)pen.x + (float)m_glyphs[ch].width) / (float)m_atlasWidth;
		m_glyphs[ch].uvTop		= (float)pen.y / (float)m_atlasHeight;
		m_glyphs[ch].uvBottom	= ((float)pen.y + (float)m_glyphs[ch].height) / (float)m_atlasHeight;

		unsigned int width = face->glyph->bitmap.width;
		unsigned int height = face->glyph->bitmap.rows;

		for (UINT row = 0; row < height; row++)
		{
			for (UINT col = 0; col < width; col++)
			{
				BYTE value = face->glyph->bitmap.buffer[col + row * width];

				// �̰� ������...
				UINT atlas_x = pen.x + col;
				UINT atlas_y = pen.y + row;

				pTexels[atlas_x + atlas_y * m_atlasWidth] = value;
			}
		}
	}
}