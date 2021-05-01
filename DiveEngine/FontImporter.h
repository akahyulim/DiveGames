#pragma once
#include "Dive_Font.h"
//#include "External/freetype/freetype.h"
#include "External/ft2build.h"
#include FT_FREETYPE_H

namespace Dive
{
	class FontImporter
	{
	public:
		FontImporter();
		~FontImporter();

		bool ImporterFile(Dive_Font* pFont, const std::string& filepath);

	private:
	private:
		FT_LibraryRec_* m_library = nullptr;
	//	FT_StrokerRec_* m_stroker = nullptr;		// 이게 왜 없을까?
		// 헤더 인클루드도 되고 컴파일도 되는데 위 객체를 못찾겠다.
		// 그리고 복붙한 부분에도 참조 오류가 나는 것들이 많다.
	};
}