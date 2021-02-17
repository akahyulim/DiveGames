#include "DivePch.h"
#include "Dive_Texture.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "../Graphics.h"


namespace Dive
{
	Dive_Texture::Dive_Texture(Context * context)
		: Resource(context),
		m_shaderResourceView(nullptr),
		m_renderTargetView(nullptr),
		m_depthStencilView(nullptr)
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
	
	// 오직 Engine Format만 다룬다.
	// 따라서 Mipmap 생성은 직접하지 않는다.
	bool Dive_Texture::LoadFromFile(const std::string & filepath)
	{
		// 엔진 포멧 확인
		{

		}

		// file을 읽어 data 저장
		{

		}

		// data의 종류에 따라 Texture2D, ShaderResourceView, RenderTargetView 또는 Cube Texture 생성
		// data(eResourceType??)를 참조하여 CreateRenderTexture 혹은 CreateCubeTexture를 호출하는 것이 가장 이상적
		// 하지만  현재 구현상황에서는 직접 Texture와 View를 생성해야 한다.

		// Texture2D
		{
			// mipmap data를 직접 가지고 있을 수 있다.
			// CreateRenderTexture가 아니라 직접 멤버 변수를 세팅한 후 View 생성 함수를 호출해야 한다.

			if (!createTexture2D())				return false;
			if (!createShaderResourceView())	return false;
			if (!createRenderTargetView())		return false;	// depthStencilView도 파일에 저장한 걸 읽을 수 있을 것 같은데...
		}
		// Cube Texture
		{
			// array data를 가진다.
			// mipmap data도 가질 수 있다.
		}

		return true;
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

		if (!createTexture2D(generateMipmaps))	return false;
		if (!createShaderResourceView())		return false;
		if (!createRenderTargetView())			return false;
		

		if (generateMipmaps)
		{
			auto deviceContext = GetSubsystem<Graphics>()->GetRHIContext();
			deviceContext->GenerateMips(m_shaderResourceView);
			
			D3D11_TEXTURE2D_DESC desc;
			m_texture->GetDesc(&desc);
			m_mipLevels = desc.MipLevels;
		}

		m_data.clear();
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
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

		setViewport();

		if (!createTexture2D())					return false;
		if (!createShaderResourceView())		return false;
		if (!createDepthStencilView(readOnly))	return false;

		SAFE_RELEASE(m_texture);

		return true;
	}

	bool Dive_Texture::CreateCubeTexture(unsigned int width, unsigned height, DXGI_FORMAT format)
	{
		m_width		= width;
		m_height	= height;
		m_arraySize = 6;
		m_mipLevels = 1;
		m_format	= format;
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	// depth stencil 일수도 있다. 근데 어따 쓰는지는 모르겠다.

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

			CORE_TRACE("{0:d} x {1:d}, {2:d} : {3:d}", desc.Width, desc.Height, desc.MipLevels, m_mipLevels);

			// mipmap별로 접근할 순 없나? 크기 확인도 할 겸...
		}
	}

	// load to file에서 mipmap 개수는 정해져 있으므로, 오버 라이드 해야 한다.
	// 일단 Texture Array는 지원하지 않는다.
	bool Dive_Texture::createTexture2D(bool generateMipmaps)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width				= m_width;
		desc.Height				= m_height;
		desc.Format				= m_format;
		desc.Usage				= m_data.empty() ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
		desc.BindFlags			= m_bindFlags;
		desc.MipLevels			= m_mipLevels;
		desc.ArraySize			= m_arraySize;
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags			= (m_mipLevels == 0 && generateMipmaps) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		desc.CPUAccessFlags		= 0;

		std::vector<D3D11_SUBRESOURCE_DATA> datas;
		if (m_mipLevels > 1)
		{
			for (unsigned int i = 0; i < m_mipLevels; i++)
			{
				D3D11_SUBRESOURCE_DATA& data	= datas.emplace_back(D3D11_SUBRESOURCE_DATA{});
				data.pSysMem					= i < m_data.size() ? m_data[i].data() : nullptr;
				data.SysMemPitch;	// data load시 pitch만 계산하면 되지 않을까?
				data.SysMemSlicePitch			= 0;
			}
		}

		if (FAILED(m_device->CreateTexture2D(&desc, datas.empty()? nullptr : datas.data(), &m_texture)))
		{
			CORE_ERROR("");
			return false;
		}

		// 생성 후에는 m_data도 필요없다.

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