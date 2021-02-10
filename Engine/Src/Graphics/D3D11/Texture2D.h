#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	class Texture2D : public Texture
	{
	public:
		Texture2D(Context* context, bool generateMipmaps = true)
			: Texture(context)
		{
			m_resourceType	= eResourceType::Texture2D;
			m_bindFlags		= D3D11_BIND_SHADER_RESOURCE;
		}
		
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly = false, std::string name = "")
			: Texture(context)
		{
			m_resourceName	= std::move(name);
			m_resourceType	= eResourceType::Texture2D;

			m_width			= width;
			m_height		= height;
			m_format		= format;
			m_channelCount	= GetChannelCount(format);
			// m_bitsPerChannel도 다시 계산해야 하지 않나?
			m_bindFlags		= D3D11_BIND_SHADER_RESOURCE;
			m_bindFlags		|= IsDepthFormat(format) ? D3D11_BIND_DEPTH_STENCIL : D3D11_BIND_RENDER_TARGET;
			m_arraySize		= 1;
			m_mipLevels		= 1;
			
			createTextureViews(readOnly);
		}

		~Texture2D() = default;

	private:
		bool createTextureViews(bool readOnly = false);
	};
}