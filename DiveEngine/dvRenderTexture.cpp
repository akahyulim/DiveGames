#include "dvRenderTexture.h"

// �ϴ� ������ ���ķ� �̷��.
// �� �ؽ��ĸ� GBuffer�� Ȱ���� �� ����.
// �ݻ�Ǵ� 2�����̳� ȯ�� ���ο� Cubemap�� Ȱ��Ǵ� �ؽ��Ĵ�.
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