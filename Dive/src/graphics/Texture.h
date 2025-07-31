#pragma once
#include "resource/Resource.h"

namespace Dive
{
	class Texture : public Resource
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual bool Create() = 0;
		virtual void Release();

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		DXGI_FORMAT GetFormat() const { return m_format; }
		bool IsGenerateMips() const { return m_useMips; }
		uint32_t GetMipLevels() const { return m_mipLevels; }

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

		static uint32_t CalculateMipmapLevels(uint32_t width, uint32_t height);
		static uint32_t GetPixelSize(DXGI_FORMAT format);
		static bool CanGenerateMips(DXGI_FORMAT format);

	protected:
		uint32_t m_width = 1;
		uint32_t m_height = 1;

		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		
		uint32_t m_mipLevels = 1;
		bool m_useMips = false;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	};
}
