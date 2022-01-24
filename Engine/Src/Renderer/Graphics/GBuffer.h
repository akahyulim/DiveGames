#pragma once

namespace Dive
{
	class GBuffer
	{
	public:
		GBuffer();
		~GBuffer();

		bool Initialize(ID3D11Device* pDevice, unsigned int width, unsigned int height);
		void Clear();

		void PreRender(ID3D11DeviceContext* pImmediateContext);
		void PostRender(ID3D11DeviceContext* pImmediateContext);
		void PrepareForUnpack(ID3D11DeviceContext* pImmediateContext);

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		// rt별로 srv 리턴
		ID3D11ShaderResourceView* GetShaderResourceViewRT0() { return m_pShaderResourceViewRT0.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceViewRT1() { return m_pShaderResourceViewRT1.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceViewRT2() { return m_pShaderResourceViewRT2.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceViewDepthStencil() { return m_pShaderResourceViewDepthStencil.Get(); }
		

	private:
	private:
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;

		// rt0
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResourceRT0;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceViewRT0;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetViewRT0;

		// rt1
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResourceRT1;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceViewRT1;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetViewRT1;

		// rt2
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResourceRT2;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceViewRT2;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetViewRT2;

		// rt3

		// depth stencil buffer and state
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResourceDepthStencil;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceViewDepthStencil;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilViewReadOnly;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	};
}