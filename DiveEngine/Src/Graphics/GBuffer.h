#pragma once

namespace Dive
{
	class RenderTexture;

	class GBuffer
	{
	public:
		GBuffer();
		~GBuffer();

		bool Create(unsigned int width, unsigned int height);
		void Destroy();

		void PreRender();
		void PostRender();
		void PrepareForUnpack();

		ID3D11DepthStencilView* GetDepthStencilView() const;
		ID3D11DepthStencilView* GetDepthSteniclViewReadOnly() const;

		ID3D11ShaderResourceView* GetDepthStencilSRV() const;
		ID3D11ShaderResourceView* GetColorSpecIntensitySRV() const;
		ID3D11ShaderResourceView* GetNormalSRV() const;
		ID3D11ShaderResourceView* GetSpecPowerSRV() const;

		RenderTexture* GetDepthStencilTexture() const;
		RenderTexture* GetColorSpecIntTexture() const;
		RenderTexture* GetNormalTexture() const;
		RenderTexture* GetSpecPowTexture() const;

	private:
		RenderTexture* m_pDepthStencil;
		RenderTexture* m_pColorSpecIntensity;
		RenderTexture* m_pNormal;
		RenderTexture* m_pSpecPower;

		ID3D11Buffer* m_pGBufferUnpackCB;
		ID3D11DepthStencilState* m_pDepthStencilState;
	};
}