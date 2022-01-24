#include "divepch.h"
#include "Texture.h"
#include "../../Core/DiveCore.h"
#include "../../Helper/FileSystemHelper.h"
#include "../Renderer.h"
#include "../../Helper/Log.h"


// 1. 일단 LoadFromFile부터 다듬자.
// - DirectXTex가 wstring을 사용한다. 따라서 관련 함수를 따로 만들던가 string <-> wstring 변환 함수를 만들어야 한다.
// - 이후 GBuffer용 리소스 생성을 생성자 인터페이스를 만들어보자.
// 2. 리소스 포인터를 어떻게 관리할 지 좀 더 생각해보자.
// - Microsoft::WRL::ComPtr이 unique ptr도 아닌 것 같고 사용 방법도 shared ptr과 비슷하니 raw pointer가 나을 것 같다.
// 3. Device 객체는 생성자를 통해 전달받아 멤버 변수로 관리하는 편이 나을 것 같다.
// - 좀 더 생각해보던가.. 이 객체를 누가 생성하느냐에 따라 달려있다. 물론 Renderer가 생성하겠지...
namespace Dive
{
	Texture::Texture(ID3D11Device* pDevice)
		: m_pDevice(nullptr) 
	{
		DV_ASSERT(pDevice != nullptr);
		m_pDevice = pDevice;
	}

	// 문제없이 사용가능 하다.
	// map / unmap 역시 함수화하는게 맞는가?
	Texture::Texture(unsigned int width, unsigned int height)
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		// 크기 확인 필요
		m_Width = width;
		m_Height = height;
		m_Format = DXGI_FORMAT_R8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;	// 사실 4채널이 필요없다.

		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));	// 의외로 중요하다.
			desc.Format = m_Format;
			desc.Width = m_Width;
			desc.Height = m_Height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.SampleDesc.Count = 1;

			auto hr = pDevice->CreateTexture2D(&desc, nullptr, m_pTexture2D.GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		// 바로 생성하는 게 맞는 것 같다.
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;

			auto hr = pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &desc, m_pShaderResourceView.GetAddressOf());
			assert(SUCCEEDED(hr));
		}
	}

	Texture::Texture(const std::wstring& filepath, bool generateMips)
	{
		// 파일 존재 여부

		HRESULT hr = E_FAIL;

		DirectX::ScratchImage image;

		// 포멧 확인 후 로드 함수 호출
		hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));

		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		if (generateMips)
		{
			DirectX::ScratchImage mipChain;

			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
			assert(SUCCEEDED(hr));

			hr = DirectX::CreateShaderResourceView(pDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), m_pShaderResourceView.GetAddressOf());
			assert(SUCCEEDED(hr));

			setMetaData(mipChain.GetMetadata());
		}
		else
		{
			hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), m_pShaderResourceView.GetAddressOf());
			assert(SUCCEEDED(hr));

			setMetaData(image.GetMetadata());
		}

		// 이름 저장...(wstring에서 string 변환 필요)
	}

	Texture::Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name)
	{
		assert(width > 0);
		assert(height > 0);

		m_Width		= width;
		m_Height	= height;
		m_Format	= format;	// DepthStencilView는 문제가 발생할 수 있다.
		m_Name		= name;

		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		// format에 맞춰 RTV, DSV 생성
		// RTV
		{
			createTexture2D(pDevice, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
			createShaderResourceView(pDevice);
			createRenderTargetView(pDevice);
		}

		// DSV는 다음에 구현하자. Format이 복잡하고, 책에서는 다른 식으로 구현한 것 같다.
	}

	// 이거 굳이 왜 wstring으로 했지...?
	// DirectXTex가 wstring을 사용하기 때문이다.
	// 포멧 변환 함수가 필요하다.
	bool Texture::LoadFromFile(const std::wstring& filepath, bool generateMips)
	{
		// 파일 존재 여부
		//if (!FileSystemHelper::FileExists(filepath))
		{
		//	CORE_ERROR("");
		//	return false;
		}

		// clear??? 

		DirectX::ScratchImage image;

		// 포멧 확인 후 로드 함수 호출
		if (FAILED(DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image)))
		{
			CORE_ERROR("");
			return false;
		}
		
		// 그냥 생성자를 통해 전달받아 멤버 변수로 관리할까.... 싶다.
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		if (generateMips)
		{
			DirectX::ScratchImage mipChain;

			if (FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain)))
			{
				CORE_ERROR("");
				return false;
			}

			if (FAILED(DirectX::CreateShaderResourceView(pDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), m_pShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			setMetaData(mipChain.GetMetadata());
		}
		else
		{
			if (FAILED(DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), m_pShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			setMetaData(image.GetMetadata());
		}

		// 이름 저장...(wstring에서 string 변환 필요)
		return true;
	}

	bool Texture::createTexture2D(ID3D11Device* pDevice, unsigned flags)
	{
		assert(pDevice != nullptr);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format				= m_Format;
		desc.Width				= m_Width;
		desc.Height				= m_Height;
		desc.BindFlags			= flags;
		desc.ArraySize			= 1;
		desc.Usage				= D3D11_USAGE_DEFAULT;	
		desc.MipLevels			= 1;
		desc.CPUAccessFlags		= 0;	// 이것도 애매하네
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags			= 0;	// 이것도 인자로 받아야 할듯?

		auto hr = pDevice->CreateTexture2D(&desc, nullptr, m_pTexture2D.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Texture::createShaderResourceView(ID3D11Device* pDevice)
	{
		assert(pDevice != nullptr);
		assert(m_pTexture2D != nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format						= m_Format;
		desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels		= 1;
		desc.Texture2D.MostDetailedMip	= 0;

		auto hr = pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &desc, m_pShaderResourceView.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Texture::createRenderTargetView(ID3D11Device* pDevice)
	{
		assert(pDevice != nullptr);

		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format				= m_Format;
		desc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		auto hr = pDevice->CreateRenderTargetView(m_pTexture2D.Get(), &desc, m_pRenderTargetView.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	void Texture::setMetaData(const DirectX::TexMetadata& data)
	{
		m_Format	= data.format;
		m_Width		= static_cast<unsigned int>(data.width);
		m_Height	= static_cast<unsigned int>(data.height);
		m_MipLevels = static_cast<unsigned int>(data.mipLevels);
		m_ArraySize = static_cast<unsigned int>(data.arraySize);
	}
}