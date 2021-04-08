#include "Texture.h"
#include "Renderer.h"
#include "Log.h"
#include "DirectXTex/DirectXTex.h"

namespace Dive
{
	bool Texture::LoadFromFile(const std::wstring& filepath)
	{
		// 파일 존재 여부

		DirectX::ScratchImage image;

		HRESULT hr = E_FAIL;

		// dds와 tga는 포멧을 확인한다.

		hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));

		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), m_pSRV.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Texture::CreateGPUResource()
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		HRESULT hResult = E_FAIL;
		
		// create buffer
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Format = m_format;
			desc.Width = m_width;
			desc.Height = m_height;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.ArraySize = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MipLevels = 0;
			desc.CPUAccessFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags = 0;

			// 이쪽... 그 중에서도 data가 문제인 것 같은데...
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_data.data();
			unsigned int value = m_width * m_channelCount * (m_bitsPerChannel / 8);
			data.SysMemPitch = m_pitch;// m_width* m_channelCount* (m_bitsPerChannel / 8);
			data.SysMemSlicePitch = 0;

			hResult = pDevice->CreateTexture2D(&desc, &data, m_pTexture2D.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		// create shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = m_format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;

			hResult = pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture2D.Get()), &desc, m_pSRV.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		return true;
	}

}