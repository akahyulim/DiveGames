#pragma once
#include "dvTexture.h"

namespace dive
{
	/*
	* 렌더 텍스처를 사용하려면 먼저 새 렌더 텍스처를 생성하고 카메라 하나를 지정해서 렌더링합니다. 
	* 그러면 일반 텍스처와 마찬가지로 머티리얼 에서 렌더 텍스처를 사용할 수 있습니다.
	* 위의 유니티 설명에 따르면 카메라에 종속되는 렌더 타겟이라는 의미다.
	* Deferred Shading에 사용할 RenderTargetView와는 성격이 다른 것 같다.
	*/
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