#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

using namespace std;
using namespace DirectX;

/*
*	m_Data�� ����...
*	�ϴ� SaveToFile()���� �����ؾ� �Ѵ�.
*	 LoadFromFile()���� ���Ϸ� ���� �о� ���� �� ��ü�� �����Ѵ�. ���� Clear�ص� �ȴ�.
*		- �׷��� ���� �� �ؽ��ĸ� �����Ϸ��Ѵٸ� �ʿ�������.
*	Load() ���� �⺻������ LoadFromFile()�� �����ϴ�.
*		- �׷��� �̸� �и� Save()�ϰ� �;����״�.
*/
namespace dive
{
	unsigned int GetChannelCount(DXGI_FORMAT format)
	{
		unsigned int channelCount = 0;

		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			channelCount = 4;
			break;
		default:
			break;
		}

		return channelCount;
	}

	bool dvTexture2D::SaveToFile(const std::string& filepath)
	{
		// ������ ���� �����ϳ�?
		// ��ư���� ���� ���� Ȯ���� �ʿ��ϴ�.

		// data�� �־�� �Ѵ�.

		return true;
	}

	bool dvTexture2D::LoadFromFile(const std::string& filepath)
	{
		// ���� Ȯ��

		// �����غ��� �� ���ٴ�. �ᱹ �����ڸ� ���� ���� �� �����͸� ���� �ִ� ���ݾ�...
		// �ϴ� �����̳� ��������.
		// �̸�, format, size
		// mipmap ���� ����? => ����Ƽ���� on / off �ۿ� ���ٰ� �Ѵ�. ���� ����� �浹�Ѵٰ�?
		// �׸��� data

		// �׳� DirectXTex�� �� �ɷ���...? �׷��ٸ� ��ü ���䵵 �ʿ����...
		// �׷� �ʹ� �����ε�...? 

		// ���� texture�� srv�� �����ϸ� ��

		return true;
	}

	// ResourceManager::Load<Texture2D>("filepath")����
	// ���� �Ľ� �����͸� ���(size, format)���� Texture ���� ��
	// Texture2D::LoadData()�� raw data�� �ް�
	// ���������� Apply�� �ؼ� srv�� �����Ѵ�.
	// => �߸� �����ߴ�.
	// SetPixel()���� Apply()�� �ʿ��ϰ�
	// LoadData()���� �ʿ� ���ٰ� �Ѵ�.
	void dvTexture2D::Apply()
	{
		// update subresource
		if(!m_Data.empty())
		{
			// ������ �˾ƾ� �Ѵ�. channel������ 4�ΰ� ����, unsigned char�� byte�� �� ������...
			// ��, format���κ��� channel count�� ����� �� �־�� �Ѵ�.
			unsigned int rowPitch = (m_Width * GetChannelCount(m_Format)) * sizeof(unsigned char);

			// mipmap�� get / set�� �Ϸ��� map�� ���� Ÿ���� �����ؾ� �Ѵ�.
			m_pDeviceContext->UpdateSubresource(m_pResource.Get(), 0, nullptr, m_Data.data(), rowPitch, 0);
		}

		// create shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format						= m_Format;
			desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip	= 0;
			desc.Texture2D.MipLevels		= -1;

			if (FAILED(m_pDevice->CreateShaderResourceView(m_pResource.Get(), &desc, m_pShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return;
			}
		}

		// generate mipmaps
		if(m_bMipmaps)
		{
			m_pDeviceContext->GenerateMips(m_pShaderResourceView.Get());

			D3D11_TEXTURE2D_DESC desc;
			m_pResource->GetDesc(&desc);
			m_MipLevels = desc.MipLevels;
		}
	}

	// ���ο��� Apply()�� ȣ���ϴ��� �ƴϸ� Apply()������ ���ְ� �� ���� �־�� �Ѵ�.
	// �ܼ��� Data�� �����ؼ� �ȵȴ�.
	// ũ��� ������ ����� ��Ÿ ������ �ٸ� �Լ��� ���� �����ؾ� �Ѵ�.
	// ���� ũ��� ���� �׸��� mipmap ���α��� �����ϴٸ� �׳� �ᵵ �ȴ�. 
	bool dvTexture2D::LoadData(const std::vector<std::byte>& data)
	{
		if (data.empty())
			return false;

		// �̿ܿ� false�� �� ������...?

		// Apply()�� ����Ϸ��� ������� �ؾ� �Ѵ�.
		// �׷��� ����...? 

		return true;
	}

	bool dvTexture2D::LoadData(const BYTE* pData)
	{
		if (pData == nullptr)
			return false;

		// �̰� m_Data�� �����Ϸ��� ũ�� ����� �Ҵ��� �ϰ�
		// memcpy�� �����ؾ� �Ѵ�.

		// subresource update
		{
			unsigned int rowPitch = (m_Width * GetChannelCount(m_Format)) * sizeof(unsigned char);

			// mipmap�� get / set�� �Ϸ��� map�� ���� Ÿ���� �����ؾ� �Ѵ�.
			m_pDeviceContext->UpdateSubresource(m_pResource.Get(), 0, nullptr, pData, rowPitch, 0);
		}

		// create shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;

			if (FAILED(m_pDevice->CreateShaderResourceView(m_pResource.Get(), &desc, m_pShaderResourceView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// generate mipmaps
		if (m_bMipmaps)
		{
			m_pDeviceContext->GenerateMips(m_pShaderResourceView.Get());

			D3D11_TEXTURE2D_DESC desc;
			m_pResource->GetDesc(&desc);
			m_MipLevels = desc.MipLevels;
		}

		return true;
	}

	// 1. GenerateMips()���� D3D11_BIND_RENDER_TARGET�� �ʿ�� �Ѵ�.
	// 2. CPU���� Read�Ϸ��� usage�� STAGING���� �����ؾ� �Ѵ�. => ���������� �������� �Է����� ������� ���Ѵٴ� ���� �ִ�.
	// => LoadData�� �Ϸ��� ��� DYNAMIC�� CPU_ACCESS_WRITE���� ����� �Ѵ�.
	// => ������ ����Ƽó�� GetPixel()�� ������ ���̳��̴�.
	// 3. ���ʿ� ������ mipmap������ dynamic & cpu_access_write ������ �ȸ����� ���̴�.
	// => BIND_RENDER_TARGET�� ������ �� ����.
	// => UpdateSubresource()�� �ذ�å�� �� �� �ִ�.
	bool dvTexture2D::createResource()
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width				= m_Width;
		desc.Height				= m_Height;
		desc.Format				= m_Format;
		desc.ArraySize			= 1;
		desc.MipLevels = m_bMipmaps ? 0 : 1;
		desc.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;
		desc.BindFlags = m_bMipmaps ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.CPUAccessFlags =  0;// D3D11_CPU_ACCESS_WRITE;	// read���� �Ϸ��� uasge�� staging�̾�� �Ѵ�.
		desc.MiscFlags = m_bMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		
		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pResource.GetAddressOf())))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}