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
		~dvRenderTexture() = default;

		bool IsCubemap() const { return m_bCubemap; }
		void SetCubmap(bool use) { m_bCubemap = use; }

		// Resize를 지원해야 하나? 아니라면 매번 새로 만들던가 shared_ptr로 관리해야 할텐데... 
		// 그런데 이미 comptr을 사용하기 시작한 이상 shared_ptr 사용은 확정되었다고 보는게 맞다.

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

		// widht, height가 또 있는데 픽셀 단위이며, 읽기와 쓰기 모두 가능하단다.

		// Texture2D를 상속해야 하나 마나 결정하지 못했다.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	};
}