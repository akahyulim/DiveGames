#pragma once
#include "Resource/Resource.h"
#include "Graphics.h"

namespace Dive
{
	class DvTexture : public Resource
	{
		DV_CLASS(DvTexture, Resource)

	public:
		DvTexture();
		virtual ~DvTexture();

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		bool IsOpaque() const { return m_bOpaque; }

		static uint32_t CalculateMipmapLevels(uint32_t width, uint32_t height, int maxLevel = -1);
		// 포멧을 가져오는 두 메서드의 이름이 애매하다.
		static DXGI_FORMAT GetShaderResourceViewFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDepthStencilViewFormat(DXGI_FORMAT format);
		static uint32_t CalcuRowPitchSize(uint32_t width, DXGI_FORMAT format);

	protected:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11Texture2D* m_pTexture;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;
		ID3D11ShaderResourceView* m_pShaderResourceView;

		uint32_t m_Width;
		uint32_t m_Height;
		// 필터 모드? 포인트, 바일니얼?, 트릴니얼 세 가지
		// read only?
		uint32_t m_MipLevels;
		bool m_bOpaque;
		
		// 밉맵 바이어스? D3D11_SAMPLER_DESC의 MipLODBias다.
		// => 이때문에 유니티에선 텍스쳐마다 샘플러를 가진다고 생각했는데 좀 에바다.

		// 랩핑 모드
		// 랩 모드 u, v, w
	};
}
