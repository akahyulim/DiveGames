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

	bool Load(const std::string& filepath, Texture* texture, bool generateMipmaps = true);

private:
private:
};