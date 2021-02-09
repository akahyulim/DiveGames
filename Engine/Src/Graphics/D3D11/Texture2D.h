#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	class Texture2D : public Texture
	{
	public:
		// load image file = shaderResourceView
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly = false, std::string name = "")
			: Texture(context)
		{
			m_resourceName	= std::move(name);
			m_resourceType	= eResourceType::Texture2D;

			m_width			= width;
			m_height		= height;
			m_format		= format;
			m_channelCount	= GetChannelCount(format);
			m_bindFlags		= D3D11_BIND_SHADER_RESOURCE;
			m_bindFlags		|= IsDepthFormat(format) ? D3D11_BIND_DEPTH_STENCIL : D3D11_BIND_RENDER_TARGET;
			
			createTextureViews(readOnly);
		}

		~Texture2D() = default;

	private:
		bool createTextureViews(bool readOnly);
	};
}