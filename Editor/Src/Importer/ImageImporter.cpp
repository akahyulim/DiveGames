#include "Dive.h"
#include "ImageImporter.h"

bool ImageImporter::Load(const std::string & filepath, Dive_Texture * texture, bool generateMips)
{
	if (!texture) return false;

	// 파일 존재 확인
	{

	}

	// 지원 포멧 여부 확인
	{

	}

	// 로드

	// mipmap 직접 구성
	if (generateMips)
	{
		if (!generateMipmaps())	return false;
	}

	// texture에 저장
	{

	}

	return true;
}

bool ImageImporter::generateMipmaps()
{
	return false;
}