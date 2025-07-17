#pragma once
#include "Texture.h"

namespace Dive
{
	enum class eDepthFormat
	{
		None = 0,
		Depth16 = 16,
		Depth24Stencil8 = 32
	};

	class RenderTexture : public Texture
	{
	public:
		RenderTexture() = delete;
		RenderTexture(UINT32 width, UINT32 height, eDepthFormat depth = eDepthFormat::None, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~RenderTexture() override;

		bool Create() override;
		void Release() override;
		
		bool Resize(UINT32 width, UINT32 height, eDepthFormat depth = eDepthFormat::None, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);

		UINT32 GetWidth() const { return m_Width; }
		UINT32 GetHeight() const { return m_Height; }

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView; }
		ID3D11ShaderResourceView* GetDepthStencilShaderResourceView() const { return m_DepthStencilShaderResourceView; }

		static constexpr eResourceType GetType() { return eResourceType::RenderTexture; }

	private:
		UINT32 m_Width = 0;
		UINT32 m_Height = 0;

		ID3D11RenderTargetView* m_RenderTargetView = nullptr;

		ID3D11Texture2D* m_DepthStencilTexture2D = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;
		ID3D11ShaderResourceView* m_DepthStencilShaderResourceView = nullptr;

		eDepthFormat m_DepthFormat = eDepthFormat::None;
	};
}
