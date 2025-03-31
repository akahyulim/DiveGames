#pragma once
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "resource/Resource.h"

namespace Dive
{
	class Texture : public Resource
	{
		DV_CLASS(Texture, Resource)

	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual bool Create() = 0;
		virtual void Release();

		DXGI_FORMAT GetFormat() const { return m_Format; }
		bool IsGenerateMips() const { return m_UseMips; }
		UINT32 GetMipLevels() const { return m_MipLevels; }

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_ShaderResourceView; }

		static UINT32 CalculateMipmapLevels(UINT32 width, UINT32 height);
		static UINT32 GetPixelSize(DXGI_FORMAT format);
		static bool CanGenerateMips(DXGI_FORMAT format);

	protected:
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		
		UINT32 m_MipLevels = 1;
		bool m_UseMips = false;

		ID3D11Texture2D* m_Texture2D = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
	};
}