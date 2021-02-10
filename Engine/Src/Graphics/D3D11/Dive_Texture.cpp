#include "DivePch.h"
#include "Dive_Texture.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "../Graphics.h"


namespace Dive
{
	Dive_Texture::Dive_Texture(Context * context)
		: Resource(context)
	{
	}

	Dive_Texture::~Dive_Texture()
	{
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_shaderResourceView);
	}
	
	bool Dive_Texture::SaveToFile(const std::string & filepath)
	{
		return false;
	}
	
	bool Dive_Texture::LoadFromFile(const std::string & filepath)
	{
		return false;
	}
	
	bool Dive_Texture::CreateRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name)
	{
		m_resourceName = std::move(name);

		m_width = width;
		m_height = height;
		m_arraySize = 1;
		m_mipLevels = 1;
		m_format = format;

		return false;
	}
	
	bool Dive_Texture::CreateDepthStencilTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly, std::string name)
	{
		m_resourceName = std::move(name);

		m_width = width;
		m_height = height;
		m_arraySize = 1;
		m_mipLevels = 1;
		m_format = format;

		return false;
	}
	
	bool Dive_Texture::CreateCubeTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name)
	{
		m_resourceName = std::move(name);

		m_width = width;
		m_height = height;
		m_arraySize = 6;
		m_mipLevels = 1;
		m_format = format;

		return false;
	}
}