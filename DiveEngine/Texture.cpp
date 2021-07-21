#include "Texture.h"
#include "Renderer.h"
#include "Log.h"

namespace dive
{
	// �������� ��밡�� �ϴ�.
	// map / unmap ���� �Լ�ȭ�ϴ°� �´°�?
	Texture::Texture(unsigned int width, unsigned int height)
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		// ũ�� Ȯ�� �ʿ�
		m_Width = width;
		m_Height = height;
		m_Format = DXGI_FORMAT_R8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;	// ��� 4ä���� �ʿ����.

		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));	// �ǿܷ� �߿��ϴ�.
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

		// �ٷ� �����ϴ� �� �´� �� ����.
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
		// ���� ���� ����

		HRESULT hr = E_FAIL;

		DirectX::ScratchImage image;

		// ���� Ȯ�� �� �ε� �Լ� ȣ��
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

		// �̸� ����...(wstring���� string ��ȯ �ʿ�)
	}

	Texture::Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name)
	{
		assert(width > 0);
		assert(height > 0);

		m_Width		= width;
		m_Height	= height;
		m_Format	= format;	// DepthStencilView�� ������ �߻��� �� �ִ�.
		m_Name		= name;

		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		// format�� ���� RTV, DSV ����
		// RTV
		{
			createTexture2D(pDevice, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
			createShaderResourceView(pDevice);
			createRenderTargetView(pDevice);
		}

		// DSV�� ������ ��������. Format�� �����ϰ�, å������ �ٸ� ������ ������ �� ����.
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
		desc.CPUAccessFlags		= 0;	// �̰͵� �ָ��ϳ�
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags			= 0;	// �̰͵� ���ڷ� �޾ƾ� �ҵ�?

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