#pragma once
#include "dvTexture.h"

namespace dive
{
	/*
	* ���� �ؽ�ó�� ����Ϸ��� ���� �� ���� �ؽ�ó�� �����ϰ� ī�޶� �ϳ��� �����ؼ� �������մϴ�. 
	* �׷��� �Ϲ� �ؽ�ó�� ���������� ��Ƽ���� ���� ���� �ؽ�ó�� ����� �� �ֽ��ϴ�.
	* ���� ����Ƽ ���� ������ ī�޶� ���ӵǴ� ���� Ÿ���̶�� �ǹ̴�.
	* Deferred Shading�� ����� RenderTargetView�ʹ� ������ �ٸ� �� ����.
	*/
	class dvRenderTexture : public dvTexture
	{
	public:
		dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth);
		dvRenderTexture(ID3D11Device* pDevice, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format);
		~dvRenderTexture() = default;

		bool IsCubemap() const { return m_bCubemap; }
		void SetCubmap(bool use) { m_bCubemap = use; }

		// Resize�� �����ؾ� �ϳ�? �ƴ϶�� �Ź� ���� ������� shared_ptr�� �����ؾ� ���ٵ�... 
		// �׷��� �̹� comptr�� ����ϱ� ������ �̻� shared_ptr ����� Ȯ���Ǿ��ٰ� ���°� �´�.

		// Get Views
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); }

	private:
		bool createShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer);
		bool createRenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer);
		bool createDepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer);

	private:
		bool m_bCubemap = false;
		bool m_bUseMipmap = false;

		// widht, height�� �� �ִµ� �ȼ� �����̸�, �б�� ���� ��� �����ϴܴ�.

		// Texture2D�� ����ؾ� �ϳ� ���� �������� ���ߴ�.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	};
}