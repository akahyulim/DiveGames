#pragma once
#include "GraphicsDevice.h"
#include "../DiveCore.h"

namespace dive
{
	class DepthStencilState
	{
	public:
		DepthStencilState(ID3D11Device* pDevice)
		{
			DV_ASSERT(pDevice != nullptr);

			// �Ű������� �̿��� desc �ϼ�
			D3D11_DEPTH_STENCIL_DESC desc;

			// ��ü ����
			m_pDevice->CreateDepthStencilState(desc, m_pState);
		}

		~DepthStencilState()
		{
			DV_RELEASE(m_pState);
		}

		// get & set
		ID3D11DepthStencilState* GetState() { return m_pState; }

	private:
	private:
		ID3D11DepthStencilState* m_pState = nullptr;
	};
}