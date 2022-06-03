#include "divepch.h"
#include "Texture2D.h"
#include "Renderer/Renderer.h"
#include "GraphicsDevice.h"
#include "Base/Base.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
	Texture2D::Texture2D()
	{
	}
	
	Texture2D::Texture2D(unsigned long long id)
		: Texture(id)
	{
	}

	Texture2D::~Texture2D()
	{
		Shutdown();
	}

	bool Texture2D::create()
	{
		Shutdown();

		if (!m_pDevice || !m_Width || !m_Height)
			return false;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Format = m_Format;
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.ArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.BindFlags = m_BindFlags;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MiscFlags = 0;
		textureDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture2D 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		if (m_BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));

			srvDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			if (FAILED(m_pDevice->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
				Shutdown();
				return false;
			}
		}

		if (m_BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));

			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			if (FAILED(m_pDevice->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_CORE_ERROR("RenderTargetView 생성에 실패하였습니다");
				Shutdown();
				return false;
			}
		}

		if (m_BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));

			dsvDesc.Format = (DXGI_FORMAT)GetDSVFormat(textureDesc.Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_CORE_ERROR("DepthStencilView 생성에 실패하였습니다.");
				Shutdown();
				return false;
			}

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			if (FAILED(m_pDevice->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_CORE_ERROR("DepthStencilViewReadOnly 생성에 실패하였습니다.");
				Shutdown();
				return false;
			}
		}

		// 이건 어디에서...?
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = static_cast<FLOAT>(m_Width);
		m_Viewport.Height = static_cast<FLOAT>(m_Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;


		return true;
	}

	bool Texture2D::SaveFromFile(const std::string& filepath)
	{
		return true;
	}

	// 직접 하지 말고, DirectX Texture 부분을 Importer로 뽑아보자.
	bool Texture2D::LoadFromFile(const std::string& filepath)
	{
		DirectX::ScratchImage img;

		std::wstring tempPath(filepath.begin(), filepath.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		HRESULT hResult = 0;
		if (_wcsicmp(ext, L".dds") == 0)
		{
			hResult = DirectX::LoadFromDDSFile(tempPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (_wcsicmp(ext, L".tga") == 0)
		{
			hResult = DirectX::LoadFromTGAFile(tempPath.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(tempPath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_CORE_WARN("{:s} 로드에 실패하였습니다.", filepath);
			return false;
		}

		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice);

		if (FAILED(DirectX::CreateShaderResourceView(pDevice, img.GetImages(), img.GetImageCount(), img.GetMetadata(), &m_pShaderResourceView)))
		{
			DV_CORE_WARN("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		const auto& metaData = img.GetMetadata();
		m_Format = metaData.format;
		m_Width = static_cast<unsigned int>(metaData.width);
		m_Height = static_cast<unsigned int>(metaData.height);

		//SetFilepath(filepath);
		//m_Name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);
		{
			m_ExportedFilepath = filepath;
			m_EngineFilepath = filepath;
			m_Name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);
		}

		return true;
	}

	bool Texture2D::CreateTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned bindFlags)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		if (!graphicsDevice.CreateTexture2D(&desc, nullptr, &m_pTexture2D))
		{
			DV_CORE_WARN("Resource Buffer 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		return true;
	}

	bool Texture2D::CreateShaderResourceView(DXGI_FORMAT format)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = -1;

		DV_ASSERT(m_pTexture2D);
		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		if (!graphicsDevice.CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &desc, &m_pShaderResourceView))
		{
			DV_CORE_WARN("ShaderResourceView 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		return true;
	}

	// Viewport도 함께 설정
	bool Texture2D::CreateRenderTargetView(DXGI_FORMAT format)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		DV_ASSERT(m_pTexture2D);
		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		if (!graphicsDevice.CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &desc, &m_pRenderTargetView))
		{
			DV_CORE_WARN("RenderTargetView 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = static_cast<FLOAT>(m_Width);
		m_Viewport.Height = static_cast<FLOAT>(m_Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		return true;
	}

	bool Texture2D::CreateDepthStencilView(DXGI_FORMAT format)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = format;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		DV_ASSERT(m_pTexture2D);
		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		if (!graphicsDevice.CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &desc, &m_pDepthStencilView))
		{
			DV_CORE_WARN("DepthStencilView 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
		if (!graphicsDevice.CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &desc, &m_pDepthStencilViewReadOnly))
		{
			DV_CORE_WARN("DepthStencilViewReadOnly 생성에 실패하였습니다.");
			Shutdown();
			return false;
		}

		return true;
	}

	// 이걸 없애면 컴파일 오류가 발생한다..
	//bool Texture2D::operator==(const Texture& other)
	//{
	//	return false;
	//}

	bool Texture2D::SetSize(int width, int height, DXGI_FORMAT format, unsigned int bindFlags)
	{
		if (width <= 0 || height <= 0)
		{
			DV_CORE_ERROR("");
			return false;
		}

		m_Width = (unsigned int)width;
		m_Height = (unsigned int)height;
		m_Format = format;
		m_BindFlags = bindFlags;

		return create();
	}

	Texture2D* Texture2D::Create(unsigned int width, unsigned int height, DXGI_FORMAT format, bool srv, const std::string& name)
	{
		auto pTex = new Texture2D();
		DV_ASSERT(pTex);

		unsigned int bindFlags = D3D11_BIND_RENDER_TARGET;
		if (srv)
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;

		if (!pTex->CreateTexture2D(width, height, format, bindFlags))
		{
			DV_DELETE(pTex);
			return nullptr;
		}

		if (!pTex->CreateRenderTargetView(format))
		{
			DV_DELETE(pTex);
			return nullptr;
		}

		if (srv)
		{
			if (!pTex->CreateShaderResourceView(format))
			{
				DV_DELETE(pTex);
				return nullptr;
			}
		}

		pTex->SetName(name);

		return pTex;
	}

	Texture2D* Texture2D::Create(unsigned int width, unsigned int height, DXGI_FORMAT texture2D, DXGI_FORMAT dsv, DXGI_FORMAT srv, const std::string& name)
	{
		auto pTex = new Texture2D();
		DV_ASSERT(pTex);

		unsigned int bindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (srv != DXGI_FORMAT_UNKNOWN)
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;

		if (!pTex->CreateTexture2D(width, height, texture2D, bindFlags))
		{
			DV_DELETE(pTex);
			return nullptr;
		}

		if (!pTex->CreateDepthStencilView(dsv))
		{
			DV_DELETE(pTex);
			return nullptr;
		}

		if (srv != DXGI_FORMAT_UNKNOWN)
		{
			if (!pTex->CreateShaderResourceView(srv))
			{
				DV_DELETE(pTex);
				return nullptr;
			}
		}

		pTex->SetName(name);

		return pTex;
	}

	Texture2D* Texture2D::Create(const std::string& path, const std::string& name)
	{
		auto pTexture = ResourceManager::GetInstance().Load<Texture2D>(path);
		if (!name.empty())
		{
			pTexture->SetName(name);
		}

		return pTexture;
	}
}