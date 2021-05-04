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

	// size�� �޴� ���� ����.
	bool Dive_Font::LoadFromFile(const std::string& filepath, FT_Long faceIndex)
	{
		// ��� ���ڸ� �ؽ��Ŀ� �����
		// �ش� ��Ʈ�� � ������ ���ڰ� �����ϴ��� �˾ƾ� �Ѵ�.

		FT_New_Face(m_libFt, filepath.c_str(), faceIndex, &m_face);

		return true;
	}
}