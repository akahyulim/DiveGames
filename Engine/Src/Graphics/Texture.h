#pragma once
#include "Resource/Resource.h"
#include "Graphics.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	class Texture : public Resource
	{
		DV_CLASS(Texture, Resource);

	public:
		Texture();
		~Texture() override = default;

		uint32_t GetWidth() const { return m_Width; }
		void SetWidth(uint32_t width);

		uint32_t GetHeight() const { return m_Height; }
		void SetHeight(uint32_t height);

		DirectX::XMFLOAT2 GetSize() const { return { static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

		DXGI_FORMAT GetFormat() const { return m_Format; }
		void SetFormat(DXGI_FORMAT format) { m_Format = format; }

		bool UseMipMap() const { return m_bUseMipMap; }

		bool IsOpaque() const { return m_bOpaque; }

		virtual ID3D11ShaderResourceView* GetShaderResourceView() = 0;

		static DXGI_FORMAT GetShaderResourceViewFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDepthStencilViewFormat(DXGI_FORMAT format);
		static uint32_t CalcuRowPitchSize(uint32_t width, DXGI_FORMAT format);

	protected:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		uint32_t m_Width;
		uint32_t m_Height;

		DXGI_FORMAT m_Format;

		bool m_bUseMipMap;
		bool m_bOpaque;
	};
}