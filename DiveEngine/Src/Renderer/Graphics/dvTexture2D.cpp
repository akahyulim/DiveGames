#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

using namespace std;
using namespace DirectX;

namespace dive
{
	// ResourceManager::Load<Texture2D>("filepath")에서
	// 파일 파싱 데이터를 기반(size, format)으로 Texture 생성 후
	// Texture2D::LoadData()로 raw data를 받고
	// 마지막으로 Apply를 해서 srv를 생성한다.
	// => 잘못 정리했다.
	// SetPixel()에만 Apply()가 필요하고
	// LoadData()에는 필요 없다고 한다.
	void dvTexture2D::Apply()
	{
		// update subresource
		if(!m_data.empty())
		{
			// 계산법을 알아야 한다. channel때문에 4인것 같고, unsigned char는 byte인 것 같은데...
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

	// 내부에서 Apply()를 호출하던가 아니면 Apply()구현을 없애고 이 곳에 넣어야 한다. 
	bool dvTexture2D::LoadData(const std::vector<std::byte>& data)
	{
		if (data.empty())
			return false;

		// 이외에 false가 될 이유는...?

		// Apply()를 사용하려면 복사부터 해야 한다.
		// 그런데 굳이...? 

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