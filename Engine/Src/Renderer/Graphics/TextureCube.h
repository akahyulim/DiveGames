#pragma once
#include "Texture.h"

namespace Dive
{
	class TextureCube : public Texture
	{
	public:
		RESOURCE_CLASS_TYPE(TextureCube)
	public:
		TextureCube();
		TextureCube(unsigned long long id);

		bool create() override;

		// �̰� �⺻������ stv���̴�.
		bool SetSize(int size, DXGI_FORMAT format, unsigned int bindFlags);

		// ��ī�� �ڽ��� ������� �ᱹ 6���� �ؽ��ĸ� ����Ʈ �ؾ� �Ѵ�.
		// �׸��� ���ĸ�ź�� ������ ���� srv�� shader���� ó���ϴ� �� �ϴ�.
		bool LoadTexture(
			const std::string& front,
			const std::string& back,
			const std::string& up,
			const std::string& down,
			const std::string& left,
			const std::string& right,
			unsigned int bindFlags,
			bool generateMips
		);

	private:
	private:
	};
}