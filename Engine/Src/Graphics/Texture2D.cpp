#include "divepch.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Resource/Image.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

#include <DirectXTex/DirectXTex.h>

/*
* ���� Load
* - Mipmap�� ������� ���� �� ShaderResourceView�� ����
* - �ƴ϶�� RenderTargetView�� �Բ� ����
* - UpdateSubresource()�� ���
* - ���� MultiSample�� ���
* 
* Texture ���� ����
* - ShaderResourceView�� RenderTargetView �� �� ����
* - Map/Unmap()�� ��� == Dynamic
* - ���� Mipmap�� MultiSample�� ���
*/
namespace Dive
{
	Texture2D* Texture2D::s_pBlackTexture = nullptr;
	Texture2D* Texture2D::s_pGrayTexture = nullptr;
	Texture2D* Texture2D::s_pRedTexture = nullptr;
	Texture2D* Texture2D::s_pWhiteTexture = nullptr;

	Texture2D::Texture2D(Context* pContext)
		: Texture(pContext),
		m_pRenderTargetView(nullptr),
		m_pLoadImage(nullptr)
	{
	}

	Texture2D::~Texture2D()
	{
		DV_DELETE(m_pLoadImage);
		DV_RELEASE(m_pRenderTargetView);
	}


	bool Texture2D::LoadFromFile(const std::string& fileName)
	{
		m_pLoadImage = new Image(m_pContext);
		if (!m_pLoadImage->LoadFromFile(fileName))
		{
			DV_DELETE(m_pLoadImage);
			return false;
		}

		// ���� smapler, mimap, sRGB ������ �ε��� �� �����ϴ�
		// SetParameters() �ʿ�
	
		if (!SetImage(m_pLoadImage))
			return false;

		DV_DELETE(m_pLoadImage);

		return true;
	}

	// �̹��� ������ �̿��� �ؽ��� ����
	// parameter ������ ���� smapler, mipmap ���θ� �߰��� ����
	// mipmap�� ������� �ʴ´ٸ� srv������ ���
	bool Texture2D::SetImage(Image* pImage)
	{
		if (!pImage)
		{
			DV_LOG_ENGINE_ERROR("Texture2D::SetImage - �߸��� Image�� ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_Width = pImage->GetWidth();
		m_Height = pImage->GetHeight();
		m_Format = pImage->GetFormat();
		m_Usage = eTextureUsage::Static;
		
		// SetParameters���� �����Ǿ�� �ϴ� �׸��
		m_bLinear = true;
		// Mipmap�� ��� 0 or 1�� ������
		// 0�̸� �ִ�, 1�̸� ������� ������ ���ϰ� �ȴ�.
		// �̴� m_RequestedMipLevels�� �����Ѵ�.
		m_RequestedMipLevels = 0;
		m_MipLevels = CheckMaxMipLevels(pImage->GetWidth(), pImage->GetHeight(), m_RequestedMipLevels);

		if (!createResources())
			return false;

		if (!SetRawTextureData(pImage->GetPixels()))
			return false;

		return true;
	}

	// cpu���� �� �� �ִ� �ؽ��� ����
	// �⺻������ rtv�� ������ �ʾƵ� ������
	// mipmap�� ����Ϸ��� ������ �� �� ����.
	// ����� �ȸ��鵵�� �ߴ�.
	bool Texture2D::SetSize(int width, int height, DXGI_FORMAT format, int mipCount, bool linear)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bLinear = linear;
		m_Usage = eTextureUsage::Dynamic;

		// �⺻������ �ϳ��� �����Ǿ�� dynamic�� �����ϴ�.
		// device ������������ ������ �� �ִٴµ� ���� �ٽ� �˾ƺ���.
		m_MipLevels = 1;

		return createResources();
	}

	// ���� �Ӹʷ����� ����
	void Texture2D::SetPixels(const std::vector<DirectX::XMINT4>& colors, int mipLevel)
	{
		if (m_Usage != eTextureUsage::Dynamic || !m_pTexture2D)
			return;
		
		if (m_Width * m_Height != static_cast<int>(colors.size()))
			return;

		// mipmap�� �����Ϸ��� 
		// ���� D3D11CalcSubresource()�� ����Ͽ� ����
		unsigned int subResource = 0;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		
		if (FAILED(m_pGraphics->GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			subResource,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource)))
		{
			DV_LOG_ENGINE_ERROR("Texture2D::SetRawTextureData - �ؽ��� �ʿ� �����Ͽ����ϴ�.");
			return;
		}

		uint8_t* pPixels = static_cast<uint8_t*>(mappedResource.pData);

		for (int row = 0; row < m_Height; ++row)
		{
			uint32_t rowStart = row * mappedResource.RowPitch;

			for (int col = 0; col < m_Width; ++col)
			{
				uint32_t colStart = col * 4;
				size_t index = row * col;

				pPixels[rowStart + colStart + 0] = static_cast<uint8_t>(colors[index].x);
				pPixels[rowStart + colStart + 1] = static_cast<uint8_t>(colors[index].y);
				pPixels[rowStart + colStart + 2] = static_cast<uint8_t>(colors[index].z);
				pPixels[rowStart + colStart + 3] = static_cast<uint8_t>(colors[index].w);
			}
		}

		m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pTexture2D), subResource);
	}

	bool Texture2D::SetRawTextureData(const uint8_t* pData)
	{
		if (!m_pTexture2D)
		{
			DV_LOG_ENGINE_ERROR("Texture2D::SetRawTextureData - �ؽ��İ� �������� �ʾ� �����͸� ������ �� �����ϴ�.");
			return false;
		}

		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("Texture2D::SetRawTextureData - ����ִ� �����͸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		const uint8_t* pSrc = pData;
		unsigned int rowPitch = GetRowPitchSize(m_Width);

		m_pGraphics->GetDeviceContext()->UpdateSubresource(
			m_pTexture2D,
			0,
			nullptr,
			pSrc,
			rowPitch,
			0);

		return true;
	}

	Texture2D* Texture2D::GetBlackTexture(Context* pContext)
	{
		if (!s_pBlackTexture)
		{
			Graphics* pGraphics = pContext->GetSubsystem<Graphics>();
			if (!pGraphics->IsInitialized())
				return nullptr;

			DirectX::XMINT2 size = pGraphics->GetSize();

			s_pBlackTexture = new Texture2D(pContext);
			s_pBlackTexture->SetSize(size.x, size.y);
			
			std::vector<DirectX::XMINT4> colors;
			colors.assign(size.x * size.y, DirectX::XMINT4(0, 0, 0, 0));

			s_pBlackTexture->SetPixels(colors);
		}

		return s_pBlackTexture;
	}

	Texture2D* Texture2D::GetGrayTexture(Context* pContext)
	{
		if (!s_pGrayTexture)
		{
			Graphics* pGraphics = pContext->GetSubsystem<Graphics>();
			if (!pGraphics->IsInitialized())
				return nullptr;

			DirectX::XMINT2 size = pGraphics->GetSize();

			s_pGrayTexture = new Texture2D(pContext);
			s_pGrayTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign(size.x * size.y, DirectX::XMINT4(128, 128, 128, 128));

			s_pGrayTexture->SetPixels(colors);
		}

		return s_pGrayTexture;
	}

	Texture2D* Texture2D::GetRedTexture(Context* pContext)
	{
		if (!s_pRedTexture)
		{
			Graphics* pGraphics = pContext->GetSubsystem<Graphics>();
			if (!pGraphics->IsInitialized())
				return nullptr;

			DirectX::XMINT2 size = pGraphics->GetSize();

			s_pRedTexture = new Texture2D(pContext);
			s_pRedTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign(size.x * size.y, DirectX::XMINT4(255, 0, 0, 0));

			s_pRedTexture->SetPixels(colors);
		}

		return s_pRedTexture;
	}

	Texture2D* Texture2D::GetWhiteTexture(Context* pContext)
	{
		if (!s_pWhiteTexture)
		{
			Graphics* pGraphics = pContext->GetSubsystem<Graphics>();
			if (!pGraphics->IsInitialized())
				return nullptr;

			DirectX::XMINT2 size = pGraphics->GetSize();

			s_pWhiteTexture = new Texture2D(pContext);
			s_pWhiteTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign(size.x * size.y, DirectX::XMINT4(255, 255, 255, 255));

			s_pWhiteTexture->SetPixels(colors);
		}

		return s_pWhiteTexture;
	}

	void Texture2D::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Texture2D>();
	}

	bool Texture2D::createResources()
	{
		DV_RELEASE(m_pTexture2D);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pRenderTargetView);

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = static_cast<UINT>(m_Width);
		texDesc.Height = static_cast<UINT>(m_Height);
		texDesc.Format = m_bLinear ? m_Format : GetSRGBFormat(m_Format);
		texDesc.ArraySize = 1;
		texDesc.MipLevels = m_MipLevels;
		texDesc.SampleDesc.Count = 1;		// ���� ��Ƽ ���ø� ������.
		texDesc.SampleDesc.Quality = 0;		// ���� ��Ƽ ���ø� ������.
		texDesc.Usage = m_Usage == eTextureUsage::Dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if(m_MipLevels > 1)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = m_MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		texDesc.CPUAccessFlags = m_Usage == eTextureUsage::Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_RELEASE(m_pTexture2D);
			DV_LOG_ENGINE_ERROR("Texture2D::createResources - Texture2D ������ �����Ͽ����ϴ�.");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = GetSRVFormat(texDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
		srvDesc.Texture2D.MipLevels = -1;// m_MipLevels;// m_Usage == eTextureUsage::Dynamic ? m_MipLevels : 1;
		//srvDesc.Texture2D.MipLevels = m_bDynamic ? 1 : -1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			DV_RELEASE(m_pShaderResourceView);
			DV_LOG_ENGINE_ERROR("Texture2D::createResources - ShaderResourceView ������ �����Ͽ����ϴ�.");
			return false;
		}

		if(texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = texDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// ��Ƽ ���ø� ������.
			rtvDesc.Texture2D.MipSlice = 0;

			if (FAILED(m_pGraphics->GetDevice()->CreateRenderTargetView(
				static_cast<ID3D11Resource*>(m_pTexture2D),
				&rtvDesc,
				&m_pRenderTargetView)))
			{
				DV_RELEASE(m_pRenderTargetView);
				DV_LOG_ENGINE_ERROR("Texture2D::createResources - RenderTargetView ������ �����Ͽ����ϴ�.");
				return false;
			}
		}
		
		return true;
	}

}