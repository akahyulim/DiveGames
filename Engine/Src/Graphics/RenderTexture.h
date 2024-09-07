#pragma once
#include "Texture.h"

namespace Dive
{
	class RenderTexture : public Texture
	{
		DV_CLASS(RenderTexture, Texture)

	public:
		RenderTexture() = default;
		RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipmap = true);
		RenderTexture(uint32_t width, uint32_t height, uint32_t depth, bool readOnly = false);
		virtual ~RenderTexture();

		bool Create();
		void Release();

		// 이건 좀 더 생각해봐야 한다.
		// 핵심은 Color와 Depth 둘 다 만들어 사용하는 경우가 필요한가이다.
		ID3D11ShaderResourceView* GetShaderResourceView() override { return nullptr; }

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11ShaderResourceView* GetColorShaderResourceView() const { return m_pColorShaderResourceView; }

		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }
		ID3D11ShaderResourceView* GetDepthShaderResourceView() const { return m_pDepthShaderResourceView; }

		DXGI_FORMAT GetColorFormat() const { return m_ColorFormat; }
		void SetColorFormat(DXGI_FORMAT format) { m_ColorFormat = format; }

		DXGI_FORMAT GetDepthFormat() const { return m_DepthFormat; }
		void SetDepthFormat(DXGI_FORMAT format) { m_DepthFormat = format; }

		uint32_t GetArraySize() const { return m_ArraySize; }
		void SetArraySize(uint32_t size) { m_ArraySize = size; }

		bool IsDepthStencilReadOnly() const { return m_bDepthStencilReadOnly; }
		void SetDepthStencilReadOnly(bool readOnly) { m_bDepthStencilReadOnly = readOnly; }

		// 임시
		void SetSize(uint32_t width, uint32_t height) { m_Width = width; m_Height = height; }

	private:
		ID3D11Texture2D* m_pColorTexture2D;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11ShaderResourceView* m_pColorShaderResourceView;

		ID3D11Texture2D* m_pDepthTexture2D;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;
		ID3D11ShaderResourceView* m_pDepthShaderResourceView;

		DXGI_FORMAT m_ColorFormat;
		DXGI_FORMAT m_DepthFormat;

		uint32_t m_ArraySize;

		bool m_bDepthStencilReadOnly;
	};
}