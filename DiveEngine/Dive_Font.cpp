#include "Dive_Font.h"

namespace Dive
{
	Dive_Font::Dive_Font()
	{
		FT_Init_FreeType(&m_libFt);
	}

	Dive_Font::~Dive_Font()
	{
		FT_Done_FreeType(m_libFt);
	}

	// size도 받는 편이 낫다.
	bool Dive_Font::LoadFromFile(const std::string& filepath, FT_Long faceIndex)
	{
		// 모든 문자를 텍스쳐에 새기기
		// 해당 폰트에 어떤 범위의 문자가 존재하는지 알아야 한다.

		FT_New_Face(m_libFt, filepath.c_str(), faceIndex, &m_face);

		return true;
	}
}