#include "Texture.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{

	Texture::Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::vector<std::vector<std::byte>>& rawData)
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		m_width = width;
		m_height = height;
		m_format = format;

		HRESULT hResult = E_FAIL;
		
		// create buffer
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Format = m_format;
			desc.Width = m_width;
			desc.Height = m_height;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.ArraySize = static_cast<UINT>(m_rawData.size());
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MipLevels = 0;
			desc.CPUAccessFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;

			hResult = pDevice->CreateTexture2D(&desc, &data, m_pTexture2D.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		// create shader resource view
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;

			hResult = pDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(m_pTexture2D.Get()), &desc, m_pSRV.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}
	}

}