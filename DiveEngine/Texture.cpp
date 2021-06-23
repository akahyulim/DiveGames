#include "Texture.h"
#include "Renderer.h"
#include "Log.h"

namespace dive
{
	// �������� ��밡�� �ϴ�.
	// map / unmap ���� �Լ�ȭ�ϴ°� �´°�?
	Texture::Texture(unsigned int width, unsigned int height)
	{
		auto device = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(device != nullptr);

		// ũ�� Ȯ�� �ʿ�
		mWidth = width;
		mHeight = height;
		mFormat = DXGI_FORMAT_R8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;	// ��� 4ä���� �ʿ����.

		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));	// �ǿܷ� �߿��ϴ�.
			desc.Format = mFormat;
			desc.Width = mWidth;
			desc.Height = mHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.SampleDesc.Count = 1;

			auto hr = device->CreateTexture2D(&desc, nullptr, mTexture2D.GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		// �ٷ� �����ϴ� �� �´� �� ����.
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = mFormat;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;

			auto hr = device->CreateShaderResourceView(mTexture2D.Get(), &desc, mShaderResourceView.GetAddressOf());
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

			hr = DirectX::CreateShaderResourceView(pDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), mShaderResourceView.GetAddressOf());
			assert(SUCCEEDED(hr));

			setMetaData(mipChain.GetMetadata());
		}
		else
		{
			hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), mShaderResourceView.GetAddressOf());
			assert(SUCCEEDED(hr));

			setMetaData(image.GetMetadata());
		}

		// �̸� ����...(wstring���� string ��ȯ �ʿ�)
	}

	Texture::Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name)
	{
		assert(width > 0);
		assert(height > 0);

		mWidth		= width;
		mHeight	= height;
		mFormat	= format;	// DepthStencilView�� ������ �߻��� �� �ִ�.
		mName		= name;

		auto device = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(device != nullptr);

		// format�� ���� RTV, DSV ����
		// RTV
		{
			createTexture2D(device, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
			createShaderResourceView(device);
			createRenderTargetView(device);
		}

		// DSV�� ������ ��������. Format�� �����ϰ�, å������ �ٸ� ������ ������ �� ����.
	}

	bool Texture::createTexture2D(ID3D11Device* device, unsigned flags)
	{
		assert(device != nullptr);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format				= mFormat;
		desc.Width				= mWidth;
		desc.Height				= mHeight;
		desc.BindFlags			= flags;
		desc.ArraySize			= 1;
		desc.Usage				= D3D11_USAGE_DEFAULT;	
		desc.MipLevels			= 1;
		desc.CPUAccessFlags		= 0;	// �̰͵� �ָ��ϳ�
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags			= 0;	// �̰͵� ���ڷ� �޾ƾ� �ҵ�?

		auto hr = device->CreateTexture2D(&desc, nullptr, mTexture2D.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Texture::createShaderResourceView(ID3D11Device* device)
	{
		assert(device != nullptr);
		assert(mTexture2D != nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format						= mFormat;
		desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels		= 1;
		desc.Texture2D.MostDetailedMip	= 0;

		auto hr = device->CreateShaderResourceView(mTexture2D.Get(), &desc, mShaderResourceView.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Texture::createRenderTargetView(ID3D11Device* device)
	{
		assert(device != nullptr);

		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format				= mFormat;
		desc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		auto hr = device->CreateRenderTargetView(mTexture2D.Get(), &desc, mRenderTargetView.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	void Texture::setMetaData(const DirectX::TexMetadata& data)
	{
		mFormat	= data.format;
		mWidth		= static_cast<unsigned int>(data.width);
		mHeight	= static_cast<unsigned int>(data.height);
		mMipLevels = static_cast<unsigned int>(data.mipLevels);
		mArraySize = static_cast<unsigned int>(data.arraySize);
	}
}