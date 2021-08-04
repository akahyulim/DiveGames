#include "dvRenderTexture.h"

// 일단 구현은 추후로 미룬다.
// 이 텍스쳐를 GBuffer에 활용할 순 없다.
// 반사되는 2차원이나 환경 맵핑용 Cubemap에 활용되는 텍스쳐다.
namespace dive
{
	dvRenderTexture::dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth)
		: dvTexture(typeid(dvRenderTexture).hash_code(), pDevice)
	{
	}

	dvRenderTexture::dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format)
		: dvTexture(typeid(dvRenderTexture).hash_code(), pDevice)
	{
	}
	
	dvRenderTexture::~dvRenderTexture()
	{
	}
}