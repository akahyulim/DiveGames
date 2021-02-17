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
		// �̰� ������ RenderTexture, DepthStenciTexture, CubeTexture�� ����ȭ�ϴ� ���̴�.
		// data�� mipmap count, format, size ���� �����Ѵ�.

		return false;
	}
	
	// ���� Engine Format�� �ٷ��.
	// ���� Mipmap ������ �������� �ʴ´�.
	bool Dive_Texture::LoadFromFile(const std::string & filepath)
	{
		// ���� ���� Ȯ��
		{

		}

		// file�� �о� data ����
		{

		}

		// data�� ������ ���� Texture2D, ShaderResourceView, RenderTargetView �Ǵ� Cube Texture ����
		// data(eResourceType??)�� �����Ͽ� CreateRenderTexture Ȥ�� CreateCubeTexture�� ȣ���ϴ� ���� ���� �̻���
		// ������  ���� ������Ȳ������ ���� Texture�� View�� �����ؾ� �Ѵ�.

		// Texture2D
		{
			// mipmap data�� ���� ������ ���� �� �ִ�.
			// CreateRenderTexture�� �ƴ϶� ���� ��� ������ ������ �� View ���� �Լ��� ȣ���ؾ� �Ѵ�.

			if (!createTexture2D())				return false;
			if (!createShaderResourceView())	return false;
			if (!createRenderTargetView())		return false;	// depthStencilView�� ���Ͽ� ������ �� ���� �� ���� �� ������...
		}
		// Cube Texture
		{
			// array data�� ������.
			// mipmap data�� ���� �� �ִ�.
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

	// ��� readOnly ������ Shader Resource�� �������� �ʾƵ� �ȴ�.
	// ������ å�� ������ Spartan�� ������ Texture�� ���� �����ϸ鼭
	// ���� DepthStencilView�� ���� ������� �Ͽ���.
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
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	// depth stencil �ϼ��� �ִ�. �ٵ� ��� �������� �𸣰ڴ�.

		setViewport();

		// texture resource�� shader resource view�� ���� �ϳ���.
		// render target view�� depth stencil view�� ���� 6����.
		
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

			// mipmap���� ������ �� ����? ũ�� Ȯ�ε� �� ��...
		}
	}

	// load to file���� mipmap ������ ������ �����Ƿ�, ���� ���̵� �ؾ� �Ѵ�.
	// �ϴ� Texture Array�� �������� �ʴ´�.
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
				data.SysMemPitch;	// data load�� pitch�� ����ϸ� ���� ������?
				data.SysMemSlicePitch			= 0;
			}
		}

		if (FAILED(m_device->CreateTexture2D(&desc, datas.empty()? nullptr : datas.data(), &m_texture)))
		{
			CORE_ERROR("");
			return false;
		}

		// ���� �Ŀ��� m_data�� �ʿ����.

		return true;
	}

	bool Dive_Texture::createShaderResourceView()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format						= (m_bindFlags & D3D11_BIND_DEPTH_STENCIL) ? getShaderResourceViewFormat() : m_format;	// ���߿� resource Type���� ��������.
		desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip	= 0;
		desc.Texture2D.MipLevels		= (m_mipLevels == 1) ? 1 : -1;	// �Ӹ��� ������ ��� ���� ���δ�. RenderTexture�� �� 0��  �����ϴ���..

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