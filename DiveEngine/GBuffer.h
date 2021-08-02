#pragma once
#include "GraphicsInclude.h"

namespace dive
{
	class GBuffer
	{
	public:
		GBuffer() = default;
		~GBuffer() = default;

		bool Initialize(unsigned int width, unsigned int height);
		void Clear();

		void PreRender(ID3D11DeviceContext* pImmediateContext);
		void PostRender(ID3D11DeviceContext* pImmediateContext);
		void PrepareForUnpack(ID3D11DeviceContext* pImmediateContext);

		ID3D11Texture2D* GetColorTexture() { return m_pTexColorSpecIntensity; }

		// 다른 get들... 꼭 필요한가? 기억이 안난다.

	private:
	private:
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
		ID3D11ShaderResourceView** m_pSpecPowerSRV = nullptr;

		ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	};
}