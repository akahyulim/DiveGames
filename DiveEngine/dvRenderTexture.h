#pragma once
#include "dvTexture.h"

namespace dive
{
	class dvRenderTexture : public dvTexture
	{
	public:
		dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth);
		dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format);
		~dvRenderTexture();

		bool IsCubemap() const { return m_bCubemap; }
		void SetCubmap(bool use) { m_bCubemap = use; }

	private:
	private:
		bool m_bCubemap = false;
		bool m_bUseMipmap = false;

		// widht, height가 또 있는데 픽셀 단위이며, 읽기와 쓰기 모두 가능하단다.

		// 둘 다 읽기 전용인데... SRV 같다.
		// 색상 버퍼
		// 깊이/스텐실 버퍼
	};
}