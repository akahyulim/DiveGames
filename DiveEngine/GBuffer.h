#pragma once
#include "GraphicsInclude.h"

namespace dive
{
	class GBuffer
	{
	public:
		GBuffer(ID3D11Device* pDevice);
		~GBuffer();

		bool Initialize(unsigned int width, unsigned int height);
		void Clear();

		void Resize(unsigned int width, unsigned int height);

		void PreRender(ID3D11DeviceContext* pImmediateContext);
		void PostRender(ID3D11DeviceContext* pImmediateContext);
		void PrepareForUnpack(ID3D11DeviceContext* pImmediateContext);

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		// 왜 이 Texture만 전달할까?
		ID3D11Texture2D* GetColorTexture() { return m_pTexColorSpecIntensity; }

		// 왜 다른 RTV는 전달함수가 없을까?
		ID3D11DepthStencilView* GetDSV() { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetReadOnlyDSV() { return m_pReadOnlyDepthStencilView; }

		ID3D11ShaderResourceView* GetDepthStencilSRV() { return m_pDepthStencilSRV; }
		ID3D11ShaderResourceView* GetColorInensitySRV() { return m_pColorSpecIntensitySRV; }
		ID3D11ShaderResourceView* GetNormalSRV() { return m_pNormalSRV; }
		ID3D11ShaderResourceView* GetSpecPowerSRV() { return m_pSpecPowerSRV; }


	private:
	private:
		ID3D11Device* m_pDevice = nullptr;

		unsigned int m_Width = 400;
		unsigned int m_Height = 300;

		// ??? buffer?

		// RenderTarget Texture
		ID3D11Texture2D* m_pTexDepthStencil = nullptr;
		ID3D11Texture2D* m_pTexColorSpecIntensity = nullptr;
		ID3D11Texture2D* m_pTexNormal = nullptr;
		ID3D11Texture2D* m_pTexSpecPower = nullptr;

		// RenderTarget Views
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pReadOnlyDepthStencilView = nullptr;
		ID3D11RenderTargetView* m_pColorSpecIntensityRTV = nullptr;
		ID3D11RenderTargetView* m_pNormalRTV = nullptr;
		ID3D11RenderTargetView* m_pSpecPowerRTV = nullptr;

		// ShaderResource Views
		ID3D11ShaderResourceView* m_pDepthStencilSRV = nullptr;
		ID3D11ShaderResourceView* m_pColorSpecIntensitySRV = nullptr;
		ID3D11ShaderResourceView* m_pNormalSRV = nullptr;
		ID3D11ShaderResourceView* m_pSpecPowerSRV = nullptr;

		ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	};
}