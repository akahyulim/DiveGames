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

		// widht, height�� �� �ִµ� �ȼ� �����̸�, �б�� ���� ��� �����ϴܴ�.

		// �� �� �б� �����ε�... SRV ����.
		// ���� ����
		// ����/���ٽ� ����
	};
}