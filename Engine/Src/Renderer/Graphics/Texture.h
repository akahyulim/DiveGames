#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	// texture interface
	class Texture : public Resource
	{
	public:
		RESOURCE_CLASS_TYPE(Texture)

	public:
		Texture();
		Texture(unsigned long long id);
		//Texture(const std::string& name = "", unsigned long long id = 0);
		virtual ~Texture() = default;

		void Shutdown();

		//virtual bool operator==(const Texture& other) = 0;

		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D; }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() { return m_pDepthStencilViewReadOnly; }

		D3D11_VIEWPORT* GetViewport() { return &m_Viewport; }

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		unsigned int GetSRGBFormat(unsigned int format);
		unsigned int GetSRVFormat(unsigned int format);
		unsigned int GetDSVFormat(unsigned int format);

	protected:
		virtual bool create() { return true; }

	protected:
		ID3D11Device* m_pDevice = nullptr;

		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		unsigned int m_BindFlags = 0;

		ID3D11Texture2D* m_pTexture2D = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly = nullptr;

		D3D11_VIEWPORT m_Viewport;
	};
}