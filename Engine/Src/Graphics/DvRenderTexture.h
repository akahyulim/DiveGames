#pragma once
#include "DvTexture.h"

namespace Dive
{
	// srv는 기본 제공
	// rtv or dsv 선택 제공
	class DvRenderTexture : public DvTexture
	{
	public:
		DvRenderTexture();
		~DvRenderTexture() override;

		bool SetRenderTarget(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		bool SetDepthStencil(int width, int height, DXGI_FORMAT format, bool readOnly = false);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }

	private:
		bool createResources() override;

	private:
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;

		bool m_bReadOnly;
	};
}