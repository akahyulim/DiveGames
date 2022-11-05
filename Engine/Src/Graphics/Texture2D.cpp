#include "divepch.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "Renderer/Renderer.h"
#include "Renderer/Viewport.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Engine/EngineEvents.h"
#include "Renderer/RendererEvents.h"
#include "IO/Log.h"
#include "IO/FileSystem.h"

#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	Texture2D::Texture2D(Context* pContext)
		: Texture(pContext)
	{
		// �׳� �����ڿ��� �� �� ������Ű��, �ڵ鷯���� usage�� Ȯ���ϴ� ���� ���� �� ����.
		// ���� �̶����� RenderTarget�� �������� �ʴµ��� �� �� �̺�Ʈ�� �޴� ������尡 �߻��Ѵ�.
		SUBSCRIBE_EVENT(eEventType::RenderTargetUpdate, EVENT_HANDLER_PARAM(OnUpdateRenderTarget));
	}

	Texture2D::~Texture2D()
	{
		Release();

		DV_LOG_ENGINE_DEBUG("Texture2D �Ҹ��� ȣ��");
	}

	void Texture2D::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Texture2D>();
	}

	bool Texture2D::LoadFromFile(const std::string& fileName)
	{
		if (!m_pGraphics)
			return false;

		DirectX::ScratchImage img;

		auto filePath = FileSystem::StringToWstring(fileName);
		auto ext = FileSystem::GetExtension(fileName);

		HRESULT hResult = 0;
		if(ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		if (ext == ".tga")
		{
			hResult = DirectX::LoadFromTGAFile(filePath.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_LOG_ENGINE_ERROR("{:s} �ε忡 �����Ͽ����ϴ�.", fileName);
			return false;
		}

		if (!SetSize(static_cast<int>(img.GetMetadata().width),
			static_cast<int>(img.GetMetadata().height),
			img.GetMetadata().format,
			eTextureUsage::Static))
			return false;

		if (!SetData(static_cast<const void*>(img.GetImages()->pixels)))
			return false;

		img.Release();

		return true;
	}

	void Texture2D::Release()
	{
		// Graphics�� SetTexture�Ǿ� ������ null�� �������...

		DV_RELEASE(m_pDepthStencilViewReadOnly);
		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	bool Texture2D::SetSize(int width, int height, DXGI_FORMAT format, eTextureUsage usage)
	{
		if (width <= 0 || height <= 0)
		{
			DV_LOG_ENGINE_ERROR("�߸��� �ؽ��� ũ�⸦ ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_Format = format;
		m_Usage = usage;
		m_Width = width;
		m_Height = height;

		Release();

		if (!m_pGraphics)
			return false;

		m_MipmapCount = GetEnableMipmap() ? CheckMaxMipmapCount(m_Width, m_Height) : 1;
		if (m_Usage == eTextureUsage::DepthStencil || m_Usage == eTextureUsage::Dynamic)
			m_MipmapCount = 1;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = static_cast<UINT>(m_Width);
		desc.Height = static_cast<UINT>(m_Height);
		desc.Format = m_Format;
		desc.ArraySize = 1;
		desc.MipLevels = m_MipmapCount;
		desc.SampleDesc.Count = 1;		// ���� ��Ƽ ���ø� ������.
		desc.SampleDesc.Quality = 0;	// ���� ��Ƽ ���ø� ������.
		desc.Usage = m_Usage == eTextureUsage::Dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_Usage == eTextureUsage::RenderTarget)
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else if (m_Usage == eTextureUsage::DepthStencil)
			desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = (m_Usage == eTextureUsage::RenderTarget && m_MipmapCount != 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		desc.CPUAccessFlags = m_Usage == eTextureUsage::Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateTexture2D(&desc, nullptr, &m_pTexture2D)))
		{
			DV_LOG_ENGINE_ERROR("Texture2D ������ �����Ͽ����ϴ�.");
			Release();
			return false;
		}

		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = GetSRVFormat(m_Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
			srvDesc.Texture2D.MipLevels = m_Usage != eTextureUsage::Dynamic ? m_MipmapCount : 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			if (FAILED(m_pGraphics->GetDevice()->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_LOG_ENGINE_ERROR("ShaderReosurceView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = m_Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
			rtvDesc.Texture2D.MipSlice = 0;

			if (FAILED(m_pGraphics->GetDevice()->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_LOG_ENGINE_ERROR("RenderTargetView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));
			dsvDesc.Format = GetDSVFormat(m_Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
			dsvDesc.Texture2D.MipSlice = 0;

			if (FAILED(m_pGraphics->GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_LOG_ENGINE_ERROR("DepthStencilView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (FAILED(m_pGraphics->GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_LOG_ENGINE_ERROR("DepthStencilViewReadOnly ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		return true;
	}

	bool Texture2D::SetData(const void* pData)
	{
		if (!m_pTexture2D)
		{
			DV_LOG_ENGINE_ERROR("�ؽ��İ� �������� �ʾ� �����͸� ������ �� �����ϴ�.");
			return false;
		}
		
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("����ִ� �����͸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		unsigned char* pSrc = (unsigned char*)pData;
		unsigned int rowPitch = GetRowPitchSize(m_Width);

		if (m_Usage == eTextureUsage::Dynamic)
		{
			unsigned int subResource = 0;

			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = nullptr;

			if (FAILED(m_pGraphics->GetDeviceContext()->Map(
				static_cast<ID3D11Resource*>(m_pTexture2D),
				subResource,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedData)))
			{
				DV_LOG_ENGINE_ERROR("�ؽ��� �ʿ� �����Ͽ����ϴ�.");
				return false;
			}

			// �̱���

			m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pTexture2D), subResource);
		}
		else
		{
			m_pGraphics->GetDeviceContext()->UpdateSubresource(
				m_pTexture2D,
				0,
				nullptr,
				pSrc,
				rowPitch,
				0);
		}

		return true;
	}

	void Texture2D::OnUpdateRenderTarget(const Event& e)
	{
		if (m_Usage != eTextureUsage::RenderTarget || !m_pRenderTargetView)
			return;

		auto* pRenderer = GetSubsystem<Renderer>();
		if (pRenderer)
			pRenderer->QueueViewportByRenderTarget(this);
	}
}