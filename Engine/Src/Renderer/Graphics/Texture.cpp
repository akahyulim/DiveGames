#include "divepch.h"
#include "Texture.h"
#include "Base/Base.h"
#include "Dive.h"
#include "Helper/FileSystem.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
	Texture::Texture()
	{
		m_pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(m_pDevice);
	}

	Texture::Texture(unsigned long long id)
		: Resource(id)
	{
		m_pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(m_pDevice);
	}

	//Texture::Texture(const std::string& name, unsigned long long id)
	//	: Resource(name, id)
	//{
	//	m_pDevice = Renderer::GetGraphicsDevice().GetDevice();
	//	DV_ASSERT(m_pDevice);
	//}

	void Texture::Shutdown()
	{
		DV_RELEASE(m_pTexture2D);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pDepthStencilViewReadOnly);
	}

	unsigned int Texture::GetSRGBFormat(unsigned int format)
	{
		if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		else if (format == DXGI_FORMAT_BC1_UNORM)
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		else if (format == DXGI_FORMAT_BC2_UNORM)
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		else if (format == DXGI_FORMAT_BC3_UNORM)
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		else
			return format;
	}
	
	unsigned int Texture::GetSRVFormat(unsigned int format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		else if (format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_R16_UNORM;
		else if (format == DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_R32_FLOAT;
		else
			return format;
	}
	
	unsigned int Texture::GetDSVFormat(unsigned int format)
	{
		if (format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		else if (format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_D16_UNORM;
		else if (DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_D32_FLOAT;
		else
			return format;
	}
}