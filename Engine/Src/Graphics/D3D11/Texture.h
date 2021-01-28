#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;
	class Graphics;

	// 일단 ID3D11UnorderedAccessView는 제외
	class Texture : public Resource
	{
	public:
		Texture(Context* context);
		virtual ~Texture();

		// load, save

		// get, set texture datas


		// get
		virtual ID3D11Resource* GetResource() = 0;
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_depthStencilView; }
		//void* GetTextureResource() const { return m_textureResource; }
		//ID3D11ShaderResourceView* GetShaderResourceView(unsigned int index = 0) { return m_shaderResourceViews.size() > index ? m_shaderResourceViews[index] : nullptr; }
		//ID3D11RenderTargetView* GetRenderTargetView(unsigned int index = 0)		{ return m_renderTargetViews.size() > index ? m_renderTargetViews[index] : nullptr; }
		//ID3D11DepthStencilView* GetDepthStencilView(unsigned int index = 0)		{ return m_depthStencilViews.size() > index ? m_depthStencilViews[index] : nullptr; }
		const D3D11_VIEWPORT& GetViewport()	const { return m_viewport; }

		unsigned int GetWidth() const { return m_width; }
		unsigned int GetHeight() const { return m_height; }

	protected:
		virtual bool createTextureAndView() { return false; }
		void setViewport(unsigned int width, unsigned int height);

	protected:
		ID3D11Device* m_renderDevice;
		
		//void* m_textureResource;
		//std::vector<ID3D11ShaderResourceView*> m_shaderResourceViews;		// 이건 하나만 만드는듯?
		// 최대 8개이므로 reserve??
		//std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
		//std::vector<ID3D11DepthStencilView*> m_depthStencilViews;
		ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
		ID3D11RenderTargetView* m_renderTargetView = nullptr;
		ID3D11DepthStencilView* m_depthStencilView = nullptr;


		D3D11_VIEWPORT m_viewport;

		// texture datas
		unsigned int m_width		= 0;
		unsigned int m_height		= 0;
		unsigned int m_bpp			= 0;	// byte per pixel
		unsigned int m_bpc			= 8;	// byte per channel
		unsigned int m_channelCount	= 4;
		unsigned int m_bindFlags	= 0;
		unsigned int m_mipLevels	= 0;
		unsigned int m_arraySize	= 1;
		DXGI_FORMAT m_format		= DXGI_FORMAT_R8G8B8A8_UNORM;

		std::vector<std::vector<std::byte>> m_data;
	};
}