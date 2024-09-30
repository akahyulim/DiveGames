#pragma once
#include "Resource/Resource.h"
#include "Graphics.h"

namespace Dive
{
	class Texture : public Resource
	{
		DV_CLASS(Texture, Resource)

	public:
		Texture();
		virtual ~Texture();

		void Release();

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		DirectX::XMFLOAT2 GetSize() const { return { static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

		DXGI_FORMAT GetFormat() const { return m_Format; }

		bool IsOpaque() const { return m_bOpaque; }

		static uint32_t CalculateMipmapLevels(uint32_t width, uint32_t height, int maxLevel = -1);
		// ������ �������� �� �޼����� �̸��� �ָ��ϴ�.
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

		DXGI_FORMAT m_Format;

		// ���� ���? ����Ʈ, ���ϴϾ�?, Ʈ���Ͼ� �� ����
		// read only?
		uint32_t m_MipLevels;
		bool m_bOpaque;
		
		// �Ӹ� ���̾? D3D11_SAMPLER_DESC�� MipLODBias��.
		// => �̶����� ����Ƽ���� �ؽ��ĸ��� ���÷��� �����ٰ� �����ߴµ� �� ���ٴ�.

		// ���� ���
		// �� ��� u, v, w
	};
}
