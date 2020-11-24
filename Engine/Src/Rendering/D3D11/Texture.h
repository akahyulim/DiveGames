#pragma once
#include "Resource/IResource.h"

namespace Dive
{
	class SystemManager;
	class RenderDevice;

	class Texture : public IResource
	{
	public:
		Texture(SystemManager* manager);
		virtual ~Texture();

		ID3D11RenderTargetView* GetRenderTargetView()			const { return m_RenderTargetView; }
		ID3D11ShaderResourceView* GetShaderResourceView()		const { return m_ShaderResourceView; }
		ID3D11DepthStencilView* GetDepthStencilView(int index)	const { return m_DepthStencilViews[index]; }

	protected:
		virtual bool createTextureResource() { return false; }

	protected:
		std::shared_ptr<RenderDevice> m_RenderDevice;

		ID3D11RenderTargetView* m_RenderTargetView		= nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView	= nullptr;

		std::vector<ID3D11DepthStencilView*> m_DepthStencilViews;

		// texture datas
		unsigned int m_Width		= 0;
		unsigned int m_Height		= 0;
		unsigned int m_Bpp			= 0;	// byte per pixel
		unsigned int m_Bpc			= 8;	// byte per channel
		unsigned int m_ArraySize	= 1;
		unsigned int m_ChannelCount	= 4;
		unsigned int m_BindFlags	= 0;
		DXGI_FORMAT m_Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
		std::vector<std::vector<std::byte>> m_Data;
	};
}