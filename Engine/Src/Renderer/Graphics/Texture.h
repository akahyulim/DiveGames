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

		bool LoadFromFile(const std::string& filepath) override;

		void Shutdown();

		//virtual bool operator==(const Texture& other) = 0;

		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D; }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() { return m_pDepthStencilViewReadOnly; }

		D3D11_VIEWPORT* GetViewport() { return &m_Viewport; }

		uint32_t GetWidth() const { return m_Width; }
		void SetWidth(const uint32_t width) { m_Width = width; }

		uint32_t GetHeight() const { return m_Height; }
		void SetHeight(const uint32_t height) { m_Height = height; }

		uint32_t GetArraySize() const { return m_ArraySize; }
		void SetArraySize(const uint32_t size) { m_ArraySize = size; }

		uint32_t GetMipLevels() const { return m_MipLevels; }
		void SetMipLevels(const uint32_t levels) { m_MipLevels = levels; }

		DXGI_FORMAT GetFormat() const { return m_Format; }
		void SetFormat(const DXGI_FORMAT format) { m_Format = format; }

		uint32_t GetRowPitch() const { return m_RowPitch; }
		void SetRowPitch(const uint32_t pitch) { m_RowPitch = pitch; }

		uint32_t GetSlicePitch() const { return m_SlicePitch; }
		void SetSlicePitch(const uint32_t pitch) { m_SlicePitch = pitch; }

		unsigned int GetSRGBFormat(unsigned int format);
		unsigned int GetSRVFormat(unsigned int format);
		unsigned int GetDSVFormat(unsigned int format);

	protected:
		virtual bool create() { return true; }

	protected:
		ID3D11Device* m_pDevice = nullptr;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		unsigned int m_BindFlags = 0;
		uint32_t m_ArraySize = 0;
		uint32_t m_MipLevels = 0;
		uint32_t m_RowPitch = 0;
		uint32_t m_SlicePitch = 0;

		ID3D11Texture2D* m_pTexture2D = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		// 배열로 바꿔야 할 수 있다.
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly = nullptr;

		D3D11_VIEWPORT m_Viewport;
	};
}