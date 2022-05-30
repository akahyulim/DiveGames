#pragma once

namespace Dive
{
	struct MipmapData
	{
		std::vector<std::byte> bytes;
	};

	struct TextureSliceData
	{
		std::vector<MipmapData> mips;
		unsigned int GetMipCount() const { static_cast<unsigned int>(mips.size()); }
	};

	class DV_Texture
	{
	public:
		DV_Texture();
		virtual ~DV_Texture();

	protected:
		bool createTexture();

	protected:
		unsigned int m_Width			= 0;
		unsigned int m_Height			= 0;
		unsigned int m_MipLevels		= 1;
		unsigned int m_ArraySize		= 1;
		unsigned int m_Flags			= 0;
		unsigned int m_ChannelCount		= 0;
		unsigned int m_BitsPerChannel	= 0;
		
		std::vector<TextureSliceData> m_Data;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		D3D11_VIEWPORT m_Viewport;

		ID3D11Texture2D* m_pTexture = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceViews[12] = { nullptr, };
		ID3D11RenderTargetView* m_pRenderTargetViews[12] = { nullptr, };
		ID3D11DepthStencilView* m_pDepthStencilViews[12] = { nullptr, };
		ID3D11DepthStencilView* m_pReadOnlyDepthStencilViews[12] = { nullptr, };
	};
}