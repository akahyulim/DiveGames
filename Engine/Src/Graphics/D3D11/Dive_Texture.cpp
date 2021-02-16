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
		m_bindFlags = 0;

		m_device = GetSubsystem<Graphics>()->GetRHIDevice();
	}

	Dive_Texture::~Dive_Texture()
	{
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_shaderResourceView);
	}
	
	bool Dive_Texture::SaveToFile(const std::string & filepath)
	{
		// 이건 생성된 RenderTexture, DepthStenciTexture, CubeTexture를 파일화하는 것이다.
		// data와 mipmap count, format, size 등을 저장한다.

		return false;
	}
	
	bool Dive_Texture::LoadFromFile(const std::string & filepath)
	{
		// 오직 Engine Format만 다룬다.
		// 따라서 Mipmap 생성은 직접하지 않는다.

		// 여기서도 Texture2D, ShaderResourceView, RenderTargetView를 생성한다.
		// 따라서 데이터를 전달받아 생성하는 함수도 생각해야 한다.

		return false;
	}
	
	bool Dive_Texture::CreateRenderTexture(unsigned int width, unsigned height, DXGI_FORMAT format, bool generateMipmaps)
	{
		m_width		= width;
		m_height	= height;
		m_arraySize = 1;
		m_mipLevels = generateMipmaps ? 0 : 1;
		m_format	= format;
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		setViewport();

		if (!createTexture2D(generateMipmaps))
		{
			return false;
		}

		if (!createShaderResourceView())
		{
			return false;
		}

		if (!createRenderTargetView())
		{
			return false;
		}

		SAFE_RELEASE(m_texture);

		return true;
	}

	// 사실 readOnly 생성시 Shader Resource를 생성하지 않아도 된다.
	// 하지만 책의 예제와 Spartan은 동일한 Texture를 통해 생성하면서
	// 각각 DepthStencilView를 따로 관리토록 하였다.
	bool Dive_Texture::CreateDepthStencilTexture(unsigned int width, unsigned height, DXGI_FORMAT format, bool readOnly)
	{
		m_width		= width;
		m_height	= height;
		m_arraySize = 1;
		m_mipLevels = 1;
		m_format	= format;
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;	// depth 일수도 있다..

		setViewport();

		if (!createTexture2D())
		{
			return false;
		}

		if (!createShaderResourceView())
		{
			return false;
		}

		if (!createDepthStencilView(readOnly))
		{
			return false;
		}

		SAFE_RELEASE(m_texture);

		return true;
	}

	bool Dive_Texture::CreateCubeTexture(unsigned int width, unsigned height, DXGI_FORMAT format)
	{
		m_width = width;
		m_height = height;
		m_arraySize = 6;
		m_mipLevels = 1;
		m_format = format;
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		setViewport();

		// texture resource와 shader resource view는 각각 하나다.
		// render target view와 depth stencil view만 각각 6개다.
		
		return true;
	}

	void Dive_Texture::PrintTextureInfo()
	{
		if (m_shaderResourceView)
		{
			ID3D11Resource* resource;
			m_shaderResourceView->GetResource(&resource);

			D3D11_TEXTURE2D_DESC desc;
			static_cast<ID3D11Texture2D*>(resource)->GetDesc(&desc);

			CORE_TRACE("{0:d} x {1:d}, {2:d}", desc.Width, desc.Height, desc.MipLevels);

			// mipmap별로 접근할 순 없나? 크기 확인도 할 겸...
		}
	}

	// load to file에서 mipmap 개수는 정해져 있으므로, 오버 라이드 해야 한다.
	bool Dive_Texture::createTexture2D(bool generateMipmaps)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width				= m_width;
		desc.Height				= m_height;
		desc.Format				= m_format;
		desc.Usage				= D3D11_USAGE_DEFAULT;	// 나중에 고쳐야 한다.
		desc.BindFlags			= m_bindFlags;
		desc.MipLevels			= m_mipLevels;		// 만들어지던 말던 최대 개수로 저장된다. 이를 다시 멤버 변수로 저장할 필요도 있다.
		desc.ArraySize			= m_arraySize;
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags			= generateMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		desc.CPUAccessFlags		= 0;

		if (FAILED(m_device->CreateTexture2D(&desc, nullptr, &m_texture)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	bool Dive_Texture::createShaderResourceView()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format						= (m_bindFlags & D3D11_BIND_DEPTH_STENCIL) ? getShaderResourceViewFormat() : m_format;	// 나중에 resource Type으로 구분하자.
		desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip	= 0;
		desc.Texture2D.MipLevels		= (m_mipLevels == 1) ? 1 : -1;	// 밉맵이 존재할 경우 하위 전부다. RenderTexture일 땐 0도  전달하던데..

		if (FAILED(m_device->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_texture), &desc, &m_shaderResourceView)))
		{
			CORE_ERROR("");
			return false;
		}

		// 음... 이걸 언제 갱신하지?
		if (m_mipLevels == 0)
		{
			auto deviceContext = GetSubsystem<Graphics>()->GetRHIContext();
			// 실제로 만들어졌는지 확인할 수가 없다.
			deviceContext->GenerateMips(m_shaderResourceView);
		}

		return true;
	}

	bool Dive_Texture::createRenderTargetView()
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format				= m_format;
		desc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		if (FAILED(m_device->CreateRenderTargetView(static_cast<ID3D11Resource*>(m_texture), &desc, &m_renderTargetView)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
	bool Dive_Texture::createDepthStencilView(bool readOnly)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		desc.Format				= getDepthStencilViewFormat();
		desc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Flags				= readOnly ? D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL : 0;
		desc.Texture2D.MipSlice = 0;

		if (FAILED(m_device->CreateDepthStencilView(static_cast<ID3D11Resource*>(m_texture), &desc, &m_depthStencilView)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	DXGI_FORMAT Dive_Texture::getShaderResourceViewFormat()
	{
		switch (m_format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_R32G8X24_TYPELESS:		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case DXGI_FORMAT_R32_TYPELESS:			return DXGI_FORMAT_R32_FLOAT;
		
		default:								return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT Dive_Texture::getDepthStencilViewFormat()
	{
		switch (m_format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:		return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case DXGI_FORMAT_R32G8X24_TYPELESS:		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case DXGI_FORMAT_R32_TYPELESS:			return DXGI_FORMAT_D32_FLOAT;

		default:								return DXGI_FORMAT_UNKNOWN;
		}
	}

	void Dive_Texture::setViewport()
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width	= static_cast<float>(m_width);
		m_viewport.Height	= static_cast<float>(m_height);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}
}