#include "Dive.h"
#include "ImageImporter.h"

bool ImageImporter::Load(const std::string & filepath, Dive_Texture * texture, bool generateMips)
{
	if (!texture) return false;

	// ���� ���� Ȯ��
	{

	}

	// ���� ���� ���� Ȯ��
	{

	}

	// �ε�

	// mipmap ���� ����
	if (generateMips)
	{
		if (!generateMipmaps())	return false;
	}

	// texture�� ����
	{

	}

	return true;
}

bool ImageImporter::generateMipmaps()
{
	return false;
}