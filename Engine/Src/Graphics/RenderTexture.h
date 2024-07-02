#pragma once
#include "Texture.h"

namespace Dive
{
	class RenderTexture : public Texture
	{
		DV_CLASS(RenderTexture, Texture);

	public:
		RenderTexture();
		~RenderTexture() override;

		bool CreateRenderTargetView(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		bool CreateDepthStencilView(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly = false);
		void Release();

		ID3D11ShaderResourceView* GetShaderResourceView() override { return m_pShaderResourceView; }
		
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }

		bool IsDepthReadOnly() const { return m_bDepthReadOnly; }

	private:
		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;

		bool m_bDepthReadOnly;
	};
}