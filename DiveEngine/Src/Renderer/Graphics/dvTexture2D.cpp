#include "dvTexture2D.h"
#include "../FileSystemHelper.h"
#include "../StringHelper.h"
#include "../Log.h"

using namespace std;
using namespace DirectX;

/*
*	m_Data에 대해...
*	일단 SaveToFile()에는 존재해야 한다.
*	 LoadFromFile()에는 파일로 부터 읽어 들인 후 객체를 생성한다. 이후 Clear해도 된다.
*		- 그런데 만약 이 텍스쳐를 수정하려한다면 필요해진다.
*	Load() 역시 기본적으로 LoadFromFile()과 동일하다.
*		- 그런데 이를 분명 Save()하고 싶어질테다.
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
		// 파일을 직접 생성하나?
		// 여튼간에 파일 유무 확인이 필요하다.

		// data가 있어야 한다.

		return true;
	}

	bool dvTexture2D::LoadFromFile(const std::string& filepath)
	{
		// 포멧 확인

		// 생각해보니 좀 에바다. 결국 생성자를 통해 만든 후 데이터를 집어 넣는 거잖아...
		// 일단 과정이나 정리하자.
		// 이름, format, size
		// mipmap 생성 여부? => 유니티에선 on / off 밖에 없다고 한다. 직접 만든건 충돌한다고?
		// 그리고 data

		// 그냥 DirectXTex로 다 될려나...? 그렇다면 자체 포멧도 필요없다...
		// 그럼 너무 날먹인데...? 

		// 이후 texture와 srv를 생성하면 끝

		return true;
	}

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
		if(!m_Data.empty())
		{
			// 계산법을 알아야 한다. channel때문에 4인것 같고, unsigned char는 byte인 것 같은데...
			// 즉, format으로부터 channel count를 계산할 수 있어야 한다.
			unsigned int rowPitch = (m_Width * GetChannelCount(m_Format)) * sizeof(unsigned char);

			// mipmap별 get / set을 하려면 map을 통해 타겟을 설정해야 한다.
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

	// 내부에서 Apply()를 호출하던가 아니면 Apply()구현을 없애고 이 곳에 넣어야 한다.
	// 단순히 Data만 전달해선 안된다.
	// 크기와 포멧을 비롯해 기타 정보를 다른 함수를 통해 전달해야 한다.
	// 물론 크기와 포멧 그리고 mipmap 여부까지 동일하다면 그냥 써도 된다. 
	bool dvTexture2D::LoadData(const std::vector<std::byte>& data)
	{
		if (data.empty())
			return false;

		// 이외에 false가 될 이유는...?

		// Apply()를 사용하려면 복사부터 해야 한다.
		// 그런데 굳이...? 

		return true;
	}

	bool dvTexture2D::LoadData(const BYTE* pData)
	{
		if (pData == nullptr)
			return false;

		// 이걸 m_Data에 저장하려면 크기 계산후 할당을 하고
		// memcpy로 복사해야 한다.

		// subresource update
		{
			unsigned int rowPitch = (m_Width * GetChannelCount(m_Format)) * sizeof(unsigned char);

			// mipmap별 get / set을 하려면 map을 통해 타겟을 설정해야 한다.
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

	// 1. GenerateMips()에서 D3D11_BIND_RENDER_TARGET를 필요로 한다.
	// 2. CPU에서 Read하려면 usage를 STAGING으로 설정해야 한다. => 파이프라인 스테이지 입력으로 사용하지 못한다는 글이 있다.
	// => LoadData를 하려면 적어도 DYNAMIC에 CPU_ACCESS_WRITE까진 해줘야 한다.
	// => 문제는 유니티처럼 GetPixel()을 구현할 것이냐이다.
	// 3. 애초에 문제는 mipmap생성과 dynamic & cpu_access_write 조합이 안먹히는 것이다.
	// => BIND_RENDER_TARGET이 문제인 것 같다.
	// => UpdateSubresource()가 해결책이 될 수 있다.
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
		desc.CPUAccessFlags =  0;// D3D11_CPU_ACCESS_WRITE;	// read까지 하려면 uasge가 staging이어야 한다.
		desc.MiscFlags = m_bMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		
		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pResource.GetAddressOf())))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}