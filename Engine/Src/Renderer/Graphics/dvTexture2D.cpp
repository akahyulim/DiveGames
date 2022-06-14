#include "divepch.h"
#include "dvTexture2D.h"
#include "Base/Base.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Image.h"

namespace Dive
{
	DvTexture2D::DvTexture2D()
	{
	}

	DvTexture2D::~DvTexture2D()
	{
		Release();
	}
	
	void DvTexture2D::Release()
	{
		// ��������� ������� ���� ã�� nullptr�� �����Ѵ�.

		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	bool DvTexture2D::LoadFromFile(const std::string& filepath)
	{
		// Begin / End Load�� ������ �ƴ϶�� ��� ������ �ʿ䰡 ����.
		auto pLoadImage = new Image;
		if (!pLoadImage->Load(filepath))
		{
			// reset
			return false;
		}

		auto bResult = SetData(pLoadImage);

		// loadImage reset

		return bResult;
	}

	bool DvTexture2D::SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage)
	{
		if (width <= 0 || height <= 0)
		{
			DV_CORE_ERROR("�߸��� Texture ũ�⸦ ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_Usage = usage;

		// 1. multiSample > 1 && autoResolve == false �� 1
		// 2. SetNumLevels()���� usage > TEXTURE_RENDERTARGET �̸� 1	=> ������� requestLevels
		// 3. Create_D3D11����	=> ���⼭���� m_MipLevels
		//	1. ChcekLevels���� requestLevels == 0 �Ǵ� ��� ������� ������ ��� ��� ���
		//	2. USAGE == TEXTURE_DEPTHSTENCIL�̸� 1
		//	3. TEXTURE_RENDERTARGET && mipLevels != 1 && multiSample == 1�̸� generate mips ����
		//	4. srv���� �� desc.Texture2D.MipLevels = usage != TEXTURE_DYNAMIC ? m_MipLevels : 1

		// autoResolve�� �Ӹ� �ڵ� ���� �Լ� ��뿩�θ� ���ϴ� �� �ѵ�...
		// ��� �����ε� �����Ѵ�.
		// ������ ���� Sample�� ������ �޾� ���߿� ����ȴ�.
		// �׷��� ���� RegenerateLevels()���� Ȯ���� ���� �ʴ´�.
		// ���� �ܺο��� GetAutoResolve()�� Ȯ���� �� RegerateLevels()�� ȣ���� �� �ִ�.

		m_MipLevels = m_Usage > eTextureUsage::TEXTURE_RENDERTARGET ? 1 : CalMipMaxLevel(m_Width, m_Height);

		return Create();
	}

	// Ư�� mipmap�� Ư�� ��ǥ ���� �ٲ۴�.
	bool DvTexture2D::SetData(uint32_t level, int x, int y, uint32_t width, uint32_t height, const void* pData)
	{
		if (!pData)
		{
			DV_CORE_ERROR("�ҽ� �����Ͱ� �������� �ʽ��ϴ�.");
			return false;
		}

		if (!m_pTexture2D)
		{
			DV_CORE_ERROR("Texture2D�� �������� �ʾ� �����͸� ������ �� �����ϴ�.");
			return false;
		}

		if(level >= m_MipLevels)
		{
			DV_CORE_ERROR("�߸��� MipLevel�� ���޹޾ҽ��ϴ�.");
			return false;
		}

		uint32_t levelWidth = GetLevelWidth(level);
		uint32_t levelHeight = GetLevelHeight(level);
		if (x < 0 || x + width  > levelWidth || y < 0 || y + height > levelHeight || width <= 0 || height <= 0)
		{
			DV_CORE_ERROR("�߸��� ũ�⸦ ���޹޾ҽ��ϴ�.");
			return false;
		}

		auto pSrc = (unsigned char*)pData;
		auto rowSize = GetRowDataSize(width);
		auto rowStart = GetRowDataSize(x);
		auto subResource = D3D11CalcSubresource(level, 0, m_MipLevels);

		if (m_Usage == eTextureUsage::TEXTURE_DYNAMIC)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = nullptr;

			auto hResult = Renderer::GetGraphicsDevice().GetImmediateContext()->Map(
				(ID3D11Resource*)m_pTexture2D,
				subResource,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedData);

			if (FAILED(hResult) || !mappedData.pData)
			{
				DV_CORE_ERROR("Texture Map�� �����Ͽ����ϴ�.");
				return false;
			}

			for (uint32_t row = 0; row < height; ++row)
			{
				memcpy(
					(unsigned char*)mappedData.pData + (row + y) * mappedData.RowPitch + rowStart,
					pSrc + row * rowSize,
					rowSize);

				Renderer::GetGraphicsDevice().GetImmediateContext()->Unmap((ID3D11Resource*)m_pTexture2D, subResource);
			}
		}

		return true;
	}

	bool DvTexture2D::SetData(Image* pImage, bool useAlpha)
	{
		if (!pImage)
		{
			DV_CORE_ERROR("Image�� �߸� ���� �޾ҽ��ϴ�.");
			return false;
		}

		// �Ӹ��� ������ ���¸� �����Ѵ�.

		return true;
	}

	bool DvTexture2D::Create()
	{
		Release();

		if (!m_Width || !m_Height)
			return false;

		// MipLevels ���
		// 1. �켱 width, height, requestedLevels?�� �̿��� ���� ����Ѵ�.
		// 2. DSV�� ����� 1�� �ٽ� �����Ѵ�.
		// 3. ���������� multiSample == 1, !TEXTURE_DYNAMIC�� ��� �ռ� ����, �ƴ϶�� 1�� �����Ѵ�.
		// TEXTURE_RENDERTARGET, level != 1, multiSample == 1 �� ��� D3D11_RESOURCE_MISC_GENERATE_MIPS�� �����Ѵ�.
		// GenerateMips() �Լ��� ���εǾ� �ִ�. rendertarget�� �׷��� ��, ���ø� ���� ȣ��ȴٴµ�...
		// rastertek�� ��� srv�� �ε��� �� ȣ���ߴ�. ��, srv�� ����ϱ� �� ȣ���ϴ� �� �ϴ�.

		// texture2d
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = (UINT)m_Width;
		texDesc.Height = (UINT)m_Height;
		texDesc.Format = m_Format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;			// �ӽ�
		texDesc.SampleDesc.Count = 1;	// �ӽ�
		texDesc.SampleDesc.Quality = 0;	// �ӽ�
		texDesc.Usage = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_Usage == eTextureUsage::TEXTURE_RENDERTARGET)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else if (m_Usage == eTextureUsage::TEXTURE_DEPTHSTENCIL)
			texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		texDesc.MiscFlags = 0;	// �ӽ� generate mips�� �� �� �ִ�.
		texDesc.CPUAccessFlags = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture ������ �����Ͽ����ϴ�.");
			Release();
			return false;
		}

		// srv
		if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = GetSRVFormat(m_Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	// �ӽ�, multi sample�� �� �� �ִ�.
			srvDesc.Texture2D.MipLevels = 1;		// �ӽ�, -1�̸� ��ü�� ����Ѵٴ� �ǹ�
			srvDesc.Texture2D.MostDetailedMip = 0;	// �ӽ�

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("ShaderReosurceView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		// rtv
		if (texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = m_Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// �ӽ�, multi sample�� �� �� �ִ�.
			rtvDesc.Texture2D.MipSlice = 0;		// �ӽ�

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_CORE_ERROR("RenderTargetView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		// dsv
		if (texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));
			dsvDesc.Format = GetDSVFormat(m_Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// �ӽ�, multi sample�� �� �� �ִ�.
			dsvDesc.Texture2D.MipSlice = 0;	// �ӽ�

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_CORE_ERROR("DepthStencilView ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_CORE_ERROR("DepthStencilViewReadOnly ������ �����Ͽ����ϴ�.");
				Release();
				return false;
			}
		}

		// temp
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = static_cast<FLOAT>(m_Width);
		m_Viewport.Height = static_cast<FLOAT>(m_Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		return true;
	}
}