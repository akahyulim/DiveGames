#include "DivePch.h"
#include "Texture.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	Texture::Texture(Context* context)
		: Resource(context),
		m_shaderResourceView(nullptr),
		m_renderTargetView(nullptr),
		m_depthStencilView(nullptr),
		m_width(0),
		m_height(0),
		m_bpp(0),
		m_bpc(0),
		m_channelCount(0),
		m_bitsPerChannel(8),
		m_bindFlags(0),
		m_mipLevels(1),
		m_arraySize(1),
		m_format(DXGI_FORMAT_UNKNOWN)
	{
		m_resourceType = eResourceType::Texture;
		m_renderDevice = GetSubsystem<Graphics>()->GetRHIDevice();
	}

	Texture::~Texture()
	{
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_shaderResourceView);
	}

	bool Texture::SaveToFile(const std::string & path)
	{
		// 가지고 있는 데이터들을 파일로 저장한다.

		return false;
	}

	bool Texture::LoadFromFile(const std::string & path)
	{
		// 파일이 존재하는지 확인
		// 파일 포멧 확인
		// 파일의 데이터를 전부 저장

		// GPU Resource 생성

		return false;
	}

	unsigned int Texture::GetChannelCount(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_D32_FLOAT:				return 1;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:		return 2;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	return 2;
		case DXGI_FORMAT_R11G11B10_FLOAT:		return 3;
		case DXGI_FORMAT_R32G32B32_FLOAT:		return 3;
		case DXGI_FORMAT_R8G8B8A8_UINT:			return 4;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:	return 4;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:	return 4;

		default: return 0;
		}
	}

	void Texture::setViewport(unsigned int width, unsigned int height)
	{
		m_viewport.Width	= static_cast<float>(width);
		m_viewport.Height	= static_cast<float>(height);
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}
}