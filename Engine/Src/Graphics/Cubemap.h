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

		// 여섯장의 텍스쳐를 로드하여 완성

	private:
		// 렌더타겟용으로 생성
		bool createColorBuffer();
		// 깊이버퍼용으로 생성
		bool createDepthBuffer();

	private:
	};
}