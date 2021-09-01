#pragma once
#include "dvTexture2D.h"

namespace dive
{
	class dvRenderTexture : public dvTexture2D
	{
	public:
		dvRenderTexture(unsigned int width, unsigned int height);
		dvRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format);

	private:
	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		//Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilViewReadOnly;
	};
}