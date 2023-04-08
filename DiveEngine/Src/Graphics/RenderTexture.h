#pragma once
#include "Texture.h"

namespace Dive
{
	// srv�� �⺻ ����
	// rtv or dsv ���� ����
	class RenderTexture : public Texture
	{	
	public:
		RenderTexture();
		~RenderTexture() override;

		bool SetSize(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		bool SetSize(int width, int height, int depth, DXGI_FORMAT format, bool readOnly = false);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }

	private:
		bool createResources() override;

	private:
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;

		int m_Depth;

		bool m_bReadOnly;
	};
}