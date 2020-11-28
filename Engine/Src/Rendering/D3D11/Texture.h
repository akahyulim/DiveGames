#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class SystemManager;
	class RenderDevice;

	class Texture : public Resource
	{
	public:
		Texture(SystemManager* manager);
		virtual ~Texture();

		ID3D11RenderTargetView* GetRenderTargetView()			const { return m_renderTargetView; }
		ID3D11ShaderResourceView* GetShaderResourceView()		const { return m_ShaderResourceView; }
		ID3D11DepthStencilView* GetDepthStencilView(int index)	const { return m_DepthStencilViews[index]; }

	protected:
		virtual bool createTextureResource() { return false; }

	protected:
		std::shared_ptr<RenderDevice> m_renderDevice;

		ID3D11RenderTargetView* m_renderTargetView		= nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView	= nullptr;

		std::vector<ID3D11DepthStencilView*> m_DepthStencilViews;

		// texture datas
		unsigned int m_width		= 0;
		unsigned int m_height		= 0;
		unsigned int m_bpp			= 0;	// byte per pixel
		unsigned int m_bpc			= 8;	// byte per channel
		unsigned int m_arraySize	= 1;
		unsigned int m_channelCount	= 4;
		unsigned int m_bindFlags	= 0;
		DXGI_FORMAT m_format		= DXGI_FORMAT_R8G8B8A8_UNORM;
		std::vector<std::vector<std::byte>> m_data;
	};
}