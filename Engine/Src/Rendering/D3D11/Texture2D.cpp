#include "DivePch.h"
#include "Texture2D.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	Texture2D::Texture2D(ID3D11Device* device, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data)
		: Texture(device)
	{
		m_Type		= eResourceType::Texture2D;
		m_Width		= width;
		m_Height	= height;
		m_Format	= format;
		m_Data		= data;
		m_BindFlags = D3D11_BIND_SHADER_RESOURCE;

		m_Viewport.Width	= static_cast<float>(width);
		m_Viewport.Height	= static_cast<float>(height);
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		if (!createTextureResource())
		{
			CORE_ERROR("");
			return;
		}
	}

	Texture2D::Texture2D(ID3D11Device* device, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name)
		: Texture(device)
	{
		m_Type		= eResourceType::Texture2D;
		m_Name		= name;
		m_Width		= width;
		m_Height	= height;
		m_Format	= format;
		m_ArraySize = arraySize;
		// flags를 인자로 받았지만 사용하지 않고 있다.
		m_BindFlags = (format == DXGI_FORMAT_D32_FLOAT) ?
					D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_Viewport.Width	= static_cast<float>(width);
		m_Viewport.Height	= static_cast<float>(height);
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		if (!createTextureResource())
		{
			return;
		}
	}

	// 함수에 함수 호출은 상당히 비효율적이다.
	// 스파르탄의 경우엔 멀티 플랫폼때문에 이런식으로 구현할 수 밖에 없다.
	bool Texture2D::createTextureResource()
	{
		if (!m_D3dDevice)
		{
			CORE_ERROR("");
			return false;
		}

		auto device = m_D3dDevice;//m_RenderDevice->GetD3dDevice();
		
		// 이걸 제거해야 하는지, 남겨둬야 하는지 모르겠다.
		ID3D11Texture2D* texture = nullptr;
		if (!createTexture(&texture))
			return false;

		// RendTargetView
		if (!createRenderTargetView(texture))
			return false;

		// DepthStencilView
		if (!createDepthStencilView(texture))
			return false;

		// ShaderResourceView
		if (!createShaderResourceView(texture))
			return false;

		// 일단 릴리즈
		SAFE_RELEASE(texture);
		
		return true;
	}

	bool Texture2D::createTexture(ID3D11Texture2D** texture)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = m_Format;
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.MipLevels = m_Data.empty() ? 1 : static_cast<UINT>(m_Data.size());
		desc.ArraySize = m_ArraySize;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = m_BindFlags;
		desc.Usage = (m_BindFlags & D3D11_BIND_RENDER_TARGET) || (m_BindFlags & D3D11_BIND_DEPTH_STENCIL) ?
			D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = 0;

		std::vector<D3D11_SUBRESOURCE_DATA> subResourceDatas;
		auto mipWidth = m_Width;
		auto mipHeight = m_Height;
		for (unsigned int i = 0; i != m_Data.size(); ++i)
		{
			if (m_Data[i].empty())
			{
				CORE_ERROR("", i);
				return false;
			}

			auto& subResourceData = subResourceDatas.emplace_back(D3D11_SUBRESOURCE_DATA{});
			subResourceData.pSysMem = m_Data[i].data();
			subResourceData.SysMemPitch = mipWidth * m_ChannelCount * (m_Bpc / 8);				// 가로 * 채널 수 / 채널의 바이트 크기
			subResourceData.SysMemSlicePitch = 0;

			mipWidth = max(mipWidth / 2, 1);
			mipHeight = max(mipHeight / 2, 1);
		}

		if (FAILED(m_D3dDevice->CreateTexture2D(&desc, subResourceDatas.data(), texture)))
		{
			CORE_ERROR("");
			return false;
		}
		return nullptr;
	}

	bool Texture2D::createRenderTargetView(ID3D11Texture2D* texture)
	{
		if (m_BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = (m_ArraySize == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = m_ArraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipSlice = 0;

			if (FAILED(m_D3dDevice->CreateRenderTargetView(static_cast<ID3D11Resource*>(texture), &desc, &m_RenderTargetView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		return true;
	}
	
	bool Texture2D::createDepthStencilView(ID3D11Texture2D* texture)
	{
		if (m_BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = (m_ArraySize == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = m_ArraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipSlice = 0;

			for (unsigned int i = 0; i != m_ArraySize; i++)
			{
				desc.Texture2DArray.FirstArraySlice = i;

				ID3D11DepthStencilView* view = nullptr;
				if (FAILED(m_D3dDevice->CreateDepthStencilView(static_cast<ID3D11Resource*>(texture), &desc, &view)))
				{
					CORE_ERROR("");
					return false;
				}
				m_DepthStencilViews.push_back(view);
			}
		}

		return true;
	}
	
	bool Texture2D::createShaderResourceView(ID3D11Texture2D* texture)
	{
		if (m_BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = m_Format;
			desc.ViewDimension = (m_ArraySize == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = m_ArraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipLevels = m_Data.empty() ? 1 : static_cast<UINT>(m_Data.size());
			desc.Texture2DArray.MostDetailedMip = 0;

			if (FAILED(m_D3dDevice->CreateShaderResourceView(static_cast<ID3D11Resource*>(texture), &desc, &m_ShaderResourceView)))
			{
				CORE_ERROR("");
				return false;
			}
		}
		return true;
	}
}
