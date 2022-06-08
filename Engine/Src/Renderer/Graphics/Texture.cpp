#include "divepch.h"
#include "Texture.h"
#include "Base/Base.h"
#include "Dive.h"
#include "Helper/FileSystem.h"
#include "Resource/ResourceManager.h"
#include "Resource/Importer/ImageImporter.h"

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

	// 단일 텍스처의 경우 그냥 사용해도 되지만
	// cube나 array일 경우 다수의 경로를 전달해야 한다.
	// 스파르탄의 경우 이때문에 좀 복잡해졌다.
	// 차라리 각각 개별 함수를 가지는 편이 나을 것 같다.
	// => 하지만 ResourceManager에서 Load가 불가능해진다.
	// 물론 타입별로 처리하면 될 것 같긴 하다.
	bool Texture::LoadFromFile(const std::string& filepath)
	{
		ImageImporter ipt;
		if (!ipt.Load(filepath, this, true))	// 밉맵 생성 임시
			return false;

		if (!create())
			return false;

		return true;
	}

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