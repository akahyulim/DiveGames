#pragma once
#include "Resource/Resource.h"
#include "GraphicsDefs.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	class Texture : public Resource
	{
		DV_CLASS(Texture, Resource);

	public:
		Texture();
		~Texture() override;

		bool IsMipLevelsDirty() const { return m_bMipLevelDirty; }
		void SetMipLevelsDirty();
		void UpdateMipLevels();

		bool IsSamplerStateDirty() const { return m_bSamplerStateDirty; }
		void UpdateSamplerState();

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }
		ID3D11SamplerState* GetSamplerState() const { return m_pSamplerState; }

		DirectX::XMFLOAT2 GetSize() const { return { (float)m_Width, (float)m_Height }; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		DXGI_FORMAT GetFormat() const { return m_Format; }

		D3D11_FILTER GetFilter() const { return m_Filter; }
		void SetFilter(D3D11_FILTER filter);

		D3D11_TEXTURE_ADDRESS_MODE SetAddressMode() const { return m_AddressMode; }
		void SetAddressMode(D3D11_TEXTURE_ADDRESS_MODE mode);

		DirectX::XMFLOAT4 GetBorderColor() const { return m_BorderColor; }
		void SetBorderColor(const DirectX::XMFLOAT4& color);

		int GetAnisoLevel() const { return m_AnisoLevel; }
		void SetAnisoLevel(int level);

		eTextureUsage GetUsage() const { return m_Usage; }

		uint32_t GetRowPitchSize(int width) const;

		bool IsOpaque() const { return m_bOpaque; }

		static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);

		static int CheckMaxMipLevels(int width, int height, int requestedMipLevels);

	protected:
		virtual bool createResources() = 0;

	protected:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11SamplerState* m_pSamplerState;	// 아무래도 Texture마다 직접 관리하는 건 부담된다.

		int m_Width;
		int m_Height;

		DXGI_FORMAT m_Format;
		bool m_bLinear;
		eTextureUsage m_Usage;

		int m_RequestedMipLevels;
		int m_MipLevels;
		bool m_bMipLevelDirty;

		D3D11_FILTER m_Filter;
		D3D11_TEXTURE_ADDRESS_MODE m_AddressMode;	// 유니티는 u, v, w를 나누어 멤버 변수로 관리한다.
		DirectX::XMFLOAT4 m_BorderColor;
		int m_AnisoLevel;
		bool m_bSamplerStateDirty;

		bool m_bOpaque;
	};
}