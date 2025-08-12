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
		RenderTexture(uint32_t width, uint32_t height, eDepthFormat depth = eDepthFormat::None, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~RenderTexture() override;

		bool Create() override;
		void Release() override;
		
		bool Resize(uint32_t width, uint32_t height);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthDSV.Get(); }
		ID3D11ShaderResourceView* GetDepthStencilShaderResourceView() const { return m_depthSRV.Get(); }

		static constexpr eResourceType GetType() { return eResourceType::RenderTexture; }

	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthDSV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthSRV;

		eDepthFormat m_depthFormat = eDepthFormat::None;
	};
}
