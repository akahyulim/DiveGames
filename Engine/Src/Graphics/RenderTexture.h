#pragma once
#include "Texture.h"

namespace Dive
{
	struct RenderTextureDescriptor
	{
		uint32_t width;
		uint32_t height;
		uint32_t mipCount;

		DXGI_FORMAT format;

		// 텍스쳐 디맨션
		// 배열 사이즈
	};

	class RenderTexture : public Texture
	{
		DV_CLASS(RenderTexture, Texture)

	public:
		RenderTexture();	// 지우기
		//RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		//RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly = false);
		//RenderTexture(const RenderTextureDescriptor& desc);
		~RenderTexture() override;	

		// 유니티는 생성자에 인자 혹은 desc를 전달하고
		// Create 함수로 생성한다.
		bool CreateRenderTargetView(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		bool CreateDepthStencilView(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly = false);
		//bool Create();
		void Release();

		ID3D11ShaderResourceView* GetShaderResourceView() override { return m_pShaderResourceView; }
		
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }

	private:
		// 유니티는 결국 render target과 depth buffer를 다른 texture, shader resource view로 관리한다.
		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;

		// descriptor
		// texture demansion
		// array size
	};
}