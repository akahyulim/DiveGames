#include "Dive_Font.h"
#include "GraphicsInclude.h"
#include "Renderer.h"

namespace Dive
{
	// �� ���, �� ������ �Լ�

	//  glyph���� ���� ū ����, ���� ũ�⸦ �����´�.
	inline void GetCharacterMaxDimension(const FT_Face& face, unsigned int& maxWidth, unsigned int& maxHeight)
	{
		unsigned int width = 0;
		unsigned int height = 0;

		for (unsigned int charCode = 32; charCode != 127; charCode++)
		{
			if (0 != FT_Load_Char(face, charCode, 0))
				continue;

			auto pBitmap = &face->glyph->bitmap;
			width = max(width, pBitmap->width);
			height = max(height, pBitmap->rows);
		}

		maxWidth = width;
		maxHeight = height;
	}

	inline void GetAtlasDeimension(const FT_Face& face, unsigned int& atlasWidth, unsigned int& atlasHeight,
		unsigned int& atlasCellWidth, unsigned int& atlasCellHeight)
	{
		unsigned int maxWidth = 0;
		unsigned int maxHeight = 0;

		GetCharacterMaxDimension(face, maxWidth, maxHeight);

		// Atlas�� Width�� ����� �����س��Ҵ�. = 512
		// Height�� ����� �ʿ��ϴ�.
		const unsigned int glyphCount = 127 - 32;
		const unsigned int glyphPerRow = 512 / maxWidth;
		const unsigned int rowCount = static_cast<unsigned int>(ceil(static_cast<float>(glyphCount) / static_cast<float>(glyphPerRow)));

		atlasWidth = 512;
		atlasHeight = maxHeight * rowCount;
		atlasCellWidth = maxWidth;
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
		//FT_Set_Pixel_Sizes(face, width, height);
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

		DirectX::XMFLOAT2 pen = DirectX::XMFLOAT2(0.0f, 0.0f);
		for (unsigned int charCode = 32; charCode < 127; charCode++)
		{
			printChar(pTexels, mappedResource.RowPitch, face, pen, charCode);

			pen.x += static_cast<float>(m_atlasCellWidth);

			if (pen.x + static_cast<float>(m_atlasCellWidth) > static_cast<float>(m_atlasWidth))
			{
				pen.x = 0.0f;
				pen.y += static_cast<float>(m_atlasCellHeight);
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

	void Dive_Font::printChar(BYTE* pTexels, unsigned int rowPitch, FT_Face& face, DirectX::XMFLOAT2& pen, FT_ULong ch)
	{
		unsigned index = FT_Get_Char_Index(face, ch);
		FT_Load_Glyph(face, index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);

		//m_glyphs[ch].offsetX = face->glyph->metrics.horiBearingX >> 6;
		//m_glyphs[ch].offsetY = face->glyph->metrics.horiBearingY >> 6;
		//m_glyphs[ch].horizontalAdvence = face->glyph->metrics.horiAdvance >> 6;
		m_glyphs[ch].width		= face->glyph->metrics.width >> 6;
		m_glyphs[ch].height		= face->glyph->metrics.height >> 6;
		m_glyphs[ch].uvLeft		= pen.x / static_cast<float>(m_atlasWidth);
		m_glyphs[ch].uvRight	= (pen.x + static_cast<float>(m_glyphs[ch].width)) / static_cast<float>(m_atlasWidth);
		m_glyphs[ch].uvTop		= pen.y / static_cast<float>(m_atlasHeight);
		m_glyphs[ch].uvBottom	= (pen.y + static_cast<float>(m_glyphs[ch].height)) / static_cast<float>(m_atlasHeight);
		

		// �� ũ��� �� ������ ���� �ٸ���?
		unsigned int width = face->glyph->bitmap.width;
		unsigned int height = face->glyph->bitmap.rows;

		for (UINT row = 0; row < height; ++row)
		{
			UINT rowStart = row * rowPitch;

			for (UINT col = 0; col < width; ++col)
			{
				UINT colStart = col;

				// Bitmap �������� ������ ã�´�.
				BYTE value = face->glyph->bitmap.buffer[row * width + col];

				// �׸��� �� ������ Texture�� �����Ѵ�.
				//pTexels[rowStart + colStart + 0] = value;
			}
		}
	}
}