#pragma once
#include "dvTexture.h"

namespace dive
{
	// ����Ƽ�� ���󰡴°� ���� �� ����.
	// �ᱹ �̰� rasterTek�� 2D Rednering�� �����ϴ�.
	// ī�޶� ���ߴ� �� �ؽ��Ŀ� ���� �׸���.(ī�޶� 2�� �̻��� ����)
	// �⺻������ color�� depth(&stencil)�� ��� ������
	// cube map���� ������ �� �ְ�, mipmap ������ ������ �� �־�� �Ѵ�.
	// ����1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// ����2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// ����3: https://www.rastertek.com/dx11tut11.html
	class dvRenderTexture : public dvTexture
	{
	public:
		dvRenderTexture(unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			// depth steicil view�� depth�� format�� ���Ѵ�.
			// �׷���... color�� depth�� ��� �����ٸ� �ᱹ depth�� srv�� �ʿ��� �� �ƴѰ�?
			// �� �κ��� rasterTek�� 2D Rendering�� �м� �� �� �� �����غ���.
		}

	private:

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	};
}