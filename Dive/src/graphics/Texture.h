#pragma once
#include "Graphics.h"
#include "core/CoreDefs.h"
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

		DXGI_FORMAT GetFormat() const { return m_Format; }
		bool IsGenerateMips() const { return m_UseMips; }
		uint32_t GetMipLevels() const { return m_MipLevels; }

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_ShaderResourceView; }

		static uint32_t CalculateMipmapLevels(uint32_t width, uint32_t height);
		static uint32_t GetPixelSize(DXGI_FORMAT format);
		static bool CanGenerateMips(DXGI_FORMAT format);

	protected:
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		
		uint32_t m_MipLevels = 1;
		bool m_UseMips = false;

		ID3D11Texture2D* m_Texture2D = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
	};
}
