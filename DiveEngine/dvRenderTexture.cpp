#include "dvRenderTexture.h"

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