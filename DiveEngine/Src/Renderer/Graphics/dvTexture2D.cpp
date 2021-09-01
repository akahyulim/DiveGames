#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

using namespace std;
using namespace DirectX;

namespace dive
{
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
		if(!m_data.empty())
		{
			// ������ �˾ƾ� �Ѵ�. channel������ 4�ΰ� ����, unsigned char�� byte�� �� ������...
			unsigned int rowPitch = (m_Width * 4) * sizeof(unsigned char);
			m_pDeviceContext->UpdateSubresource(m_pResource.Get(), 0, nullptr, m_data.data(), rowPitch, 0);
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
				return;
			}
		}

		// generate mipmaps
		if(m_bMipmaps)
		{
			m_pDeviceContext->GenerateMips(m_pShaderResourceView.Get());
		}
	}

	// ���ο��� Apply()�� ȣ���ϴ��� �ƴϸ� Apply()������ ���ְ� �� ���� �־�� �Ѵ�. 
	bool dvTexture2D::LoadData(const std::vector<std::byte>& data)
	{
		if (data.empty())
			return false;

		// �̿ܿ� false�� �� ������...?

		// Apply()�� ����Ϸ��� ������� �ؾ� �Ѵ�.
		// �׷��� ����...? 

		return true;
	}

	bool dvTexture2D::createResource()
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.Format = m_Format;
		desc.ArraySize = m_ArraySize;
		desc.MipLevels = m_bMipmaps ? 0 : 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = m_bMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pResource.GetAddressOf())))
		{
			CORE_ERROR("");
			return false;
		}

		return false;
	}
}