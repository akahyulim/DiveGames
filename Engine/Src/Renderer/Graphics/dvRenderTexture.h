#pragma once
#include "dvTexture.h"
#include "../Renderer.h"

namespace Dive
{
	// GBuffer������ �׽�Ʈ�� �����ߴ�.
	// ī�޶� ���ߴ� �� �ؽ��Ŀ� ���� �׸���.(ī�޶� 2�� �̻��� ����)
	// �⺻������ color�� depth(&stencil)�� ��� ������
	// cube map���� ������ �� �ְ�, mipmap ������ ������ �� �־�� �Ѵ�.
	// ��, �پ��� ���·� ����� �� �ִµ�, �̸� ��� �����ϰ� �����ϴ��İ� �߿��ϴ�.
	// ����1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// ����2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// ����3: https://www.rastertek.com/dx11tut11.html
	// ����4: https://www.rastertek.com/dx11tut22.html

	// ����Ƽ�� ��쿡�� Create�� ���ҽ��� �����Ѵ�.
	// �ٸ� �⺻�����δ� active�� ������ ������ �����ϰ�, �� ��쿣 Create�� �ƹ� �۾��� ���� �ʴ´�.
	class dvRenderTexture : public dvTexture
	{
	public:
		// �̷��� �����ڷ� ���۱��� �����ع����� ���� Resize�� �Լ��� �ʿ��� �� �ۿ� ���� �ȴ�.
		// ��Ÿ ���ڰ� �� ���� �����̴�.
		dvRenderTexture(unsigned int width, unsigned int height, unsigned int depth = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			m_Width = width;
			m_Height = height;
			m_Depth = depth;
			m_Format = format;

			m_bCreated = false;
		}

		// ����Ƽ interface
		bool Create();
		void Release();
		bool IsCreated() const { return m_bCreated; }

		ID3D11RenderTargetView* GetColorRenderTargetView() { return m_pColorRenderTargetView.Get(); }
		ID3D11ShaderResourceView* GetColorShaderResourceView() { return m_pColorShaderResourceView.Get(); }

		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView.Get(); }
		ID3D11ShaderResourceView* GetDepthShaderResourceView() { return m_pDepthShaderResourceView.Get(); }

		// ��Ÿ ���� Get & Set
		static dvRenderTexture* GetActive() { return m_pActive; }
		static void SetActive(dvRenderTexture* pRenderTexture);

	private:
		bool createColorResource();
		bool createDepthStencilResource();

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		// ����Ƽ�� ColorBuffer, DepthBuffer�� �����Ѵ�.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pColorResource;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pColorRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pColorShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilResource;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDepthShaderResourceView;

		bool m_bCreated = false;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		unsigned int m_Depth = 0;	// 0, 16(depth only), 24	24��°� depth 24, stencil 8�� �ǹ� �ҰŴ�.

		//bool m_bGenerateMips = false;	// �ڵ�, �Ŵ��� ���� �����̴�...
		bool m_bUseMipMap = false;		// �̰� �Ӹ� ���� �����̴�.

		unsigned int m_AntiAliasing = 1; // 1, 2, 4, 8�� ������.

		// ���� ���� ��...
		static dvRenderTexture* m_pActive;
	};
}