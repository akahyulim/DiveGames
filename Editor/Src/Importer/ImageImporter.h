#pragma once


// freeImage�� �̿��Ѵ�.
// load �Լ��� �����̴�.
// file path�� texture ��ü�� ���޹�����
// file�� ����Ʈ�� �� texture�� �����Ѵ�.
// �ɼǿ� ���� mipmap�� �����Ѵ�.

// ������ native format�� �����Ѵ�.
// ��, ����Ʈ�� �����͸� texture format���� serialze, desireialize �ؾ� �Ѵ�.

using namespace Dive;

class ImageImporter
{
public:

	bool Load(const std::string& filepath, Dive_Texture* texture, bool generateMips = true);

private:
	bool generateMipmaps();

private:
};