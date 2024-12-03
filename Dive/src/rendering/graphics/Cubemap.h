#pragma once
#include "Texture.h"

namespace Dive
{
	class Cubemap : public Texture
	{
		DV_CLASS(Cubemap, Texture)

	public:
		Cubemap(uint32_t size, DXGI_FORMAT format, bool mipChain);
		Cubemap(uint32_t size, uint32_t depth);
		virtual ~Cubemap();

		// �������� �ؽ��ĸ� �ε��Ͽ� �ϼ�

	private:
		// ����Ÿ�ٿ����� ����
		bool createColorBuffer();
		// ���̹��ۿ����� ����
		bool createDepthBuffer();

	private:
	};
}