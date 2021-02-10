#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;
	class Graphics;

	// 일단 ID3D11UnorderedAccessView는 제외
	// 아무리봐도 class를 나눌 필요까진 없다.
	class Texture : public Resource
	{
	public:
		Texture(Context* context);
		virtual ~Texture();

		// load, save
		bool SaveToFile(const std::string& path) override;
		bool LoadFromFile(const std::string& path) override;

		// get
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const		{ return m_renderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const		{ return m_depthStencilView; }
		const D3D11_VIEWPORT& GetViewport()	const				{ return m_viewport; }

		// get, set texture datas
		unsigned int GetWidth() const	{ return m_width; }
		unsigned int GetHeight() const	{ return m_height; }
		// 이외의 값들이 필요한가?


		unsigned int GetChannelCount(DXGI_FORMAT format);

		bool IsDepthFormat(DXGI_FORMAT format) const 
		{ 
			return format == DXGI_FORMAT_D32_FLOAT || format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT || format == DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

	protected:
		//virtual bool createTextureAndView() { return false; }
		void setViewport(unsigned int width, unsigned int height);

	protected:
		ID3D11Device* m_renderDevice;
		
		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11DepthStencilView* m_depthStencilView;

		D3D11_VIEWPORT m_viewport;

		// texture datas
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_bpp;				// byte per pixel
		unsigned int m_bpc;				// byte per channel
		unsigned int m_channelCount;
		unsigned int m_bitsPerChannel;
		unsigned int m_bindFlags;
		unsigned int m_mipLevels;
		unsigned int m_arraySize;
		DXGI_FORMAT m_format;

		// bpp, bpc, channelCount, bitsPerChannel 등은 거의 필요가 없는 것 같은데...
		// pitch 계산을 좀 더 효율적으로 하는 방법은 없을까?

		std::vector<std::vector<std::byte>> m_data;
	};
}