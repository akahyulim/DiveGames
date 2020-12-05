#include "DivePch.h"
#include "Texture2D.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	Texture2D::Texture2D(Dive_Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data)
		: Texture(context)
	{
		m_type		= eResourceType::Texture2D;
		m_width		= width;
		m_height	= height;
		m_format	= format;
		m_data		= data;
		m_bindFlags = D3D11_BIND_SHADER_RESOURCE;

		if (!createTextureResource())
		{
			CORE_ERROR("");
			return;
		}
	}

	Texture2D::Texture2D(Dive_Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name)
		: Texture(context)
	{
		m_type		= eResourceType::Texture2D;
		m_name		= name;
		m_width		= width;
		m_height	= height;
		m_format	= format;
		m_arraySize = arraySize;
		m_bindFlags = (format == DXGI_FORMAT_D32_FLOAT) ?
					D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		if (!createTextureResource())
		{
			CORE_ERROR("");
			return;
		}
	}

	bool Texture2D::createTextureResource()
	{
		if (!m_renderDevice || !m_renderDevice->GetD3dDevice())
		{
			CORE_ERROR("");
			return false;
		}

		auto device = m_renderDevice->GetD3dDevice();
		// 이걸 제거해야 하는지, 남겨둬야 하는지 모르겠다.
		ID3D11Texture2D* texture = nullptr;

		// Texture
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format				= m_format;
			desc.Width				= m_width;
			desc.Height				= m_height;
			desc.MipLevels			= m_data.empty() ? 1 : static_cast<UINT>(m_data.size());
			desc.ArraySize			= m_arraySize;
			desc.SampleDesc.Count	= 1;
			desc.SampleDesc.Quality = 0;
			desc.BindFlags			= m_bindFlags;
			desc.Usage				= (m_bindFlags & D3D11_BIND_RENDER_TARGET) || (m_bindFlags & D3D11_BIND_DEPTH_STENCIL) ?
									D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
			desc.MiscFlags			= 0;
			desc.CPUAccessFlags		= 0;

			std::vector<D3D11_SUBRESOURCE_DATA> subResourceDatas;
			auto mipWidth	= m_width;
			auto mipHeight = m_height;
			for (unsigned int i = 0; i != m_data.size(); ++i)
			{
				if (m_data[i].empty())
				{
					CORE_ERROR("", i);
					return false;
				}

				auto& subResourceData				= subResourceDatas.emplace_back(D3D11_SUBRESOURCE_DATA{});
				subResourceData.pSysMem				= m_data[i].data();
				subResourceData.SysMemPitch			= mipWidth * m_channelCount * (m_bpc / 8);				// 가로 * 채널 수 / 채널의 바이트 크기
				subResourceData.SysMemSlicePitch	= 0;

				mipWidth	= max(mipWidth / 2, 1);
				mipHeight	= max(mipHeight / 2, 1);
			}

			if (FAILED(device->CreateTexture2D(&desc, subResourceDatas.data(), &texture)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// RendTargetView
		if(m_bindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format							= m_format;
			desc.ViewDimension					= (m_arraySize == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize		= m_arraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipSlice		= 0;

			if (FAILED(device->CreateRenderTargetView(static_cast<ID3D11Resource*>(texture), &desc, &m_renderTargetView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// DepthStencilView
		if (m_bindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format							= m_format;
			desc.ViewDimension					= (m_arraySize == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize		= m_arraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipSlice		= 0;

			for (unsigned int i = 0; i != m_arraySize; i++)
			{
				desc.Texture2DArray.FirstArraySlice = i;

				ID3D11DepthStencilView* view = nullptr;
				if (FAILED(device->CreateDepthStencilView(static_cast<ID3D11Resource*>(texture), &desc, &view)))
				{
					CORE_ERROR("");
					return false;
				}
				m_DepthStencilViews.push_back(view);
			}
		}

		// ShaderResourceView
		if (m_bindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format							= m_format;
			desc.ViewDimension					= (m_arraySize == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize		= m_arraySize;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipLevels		= m_data.empty() ? 1 : static_cast<UINT>(m_data.size());
			desc.Texture2DArray.MostDetailedMip = 0;

			if (FAILED(device->CreateShaderResourceView(static_cast<ID3D11Resource*>(texture), &desc, &m_ShaderResourceView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// 일단 릴리즈
		SAFE_RELEASE(texture);
		
		return true;
	}
}
