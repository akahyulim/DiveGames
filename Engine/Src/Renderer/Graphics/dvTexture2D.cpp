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
		// 명시적으로 사용중인 곳을 찾아 nullptr을 전달한다.

		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	bool DvTexture2D::LoadFromFile(const std::string& filepath)
	{
		// Begin / End Load로 나눌게 아니라면 멤버 변수일 필요가 없다.
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
			DV_CORE_ERROR("잘못된 Texture 크기를 전달받았습니다.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_Usage = usage;

		// 1. multiSample > 1 && autoResolve == false 면 1
		// 2. SetNumLevels()에서 usage > TEXTURE_RENDERTARGET 이면 1	=> 여기까진 requestLevels
		// 3. Create_D3D11에서	=> 여기서부턴 m_MipLevels
		//	1. ChcekLevels에서 requestLevels == 0 또는 계산 결과보다 작으면 계산 결과 사용
		//	2. USAGE == TEXTURE_DEPTHSTENCIL이면 1
		//	3. TEXTURE_RENDERTARGET && mipLevels != 1 && multiSample == 1이면 generate mips 적용
		//	4. srv생성 중 desc.Texture2D.MipLevels = usage != TEXTURE_DYNAMIC ? m_MipLevels : 1

		// autoResolve는 밉맵 자동 생성 함수 사용여부를 뜻하는 듯 한데...
		// 멤버 변수로도 저장한다.
		// 하지만 역시 Sample에 영향을 받아 도중에 변경된다.
		// 그런데 정작 RegenerateLevels()에선 확인을 하지 않는다.
		// 물론 외부에서 GetAutoResolve()로 확인한 후 RegerateLevels()를 호출할 수 있다.

		m_MipLevels = m_Usage > eTextureUsage::TEXTURE_RENDERTARGET ? 1 : CalMipMaxLevel(m_Width, m_Height);

		return Create();
	}

	// 특정 mipmap의 특정 좌표 값을 바꾼다.
	bool DvTexture2D::SetData(uint32_t level, int x, int y, uint32_t width, uint32_t height, const void* pData)
	{
		if (!pData)
		{
			DV_CORE_ERROR("소스 데이터가 존재하지 않습니다.");
			return false;
		}

		if (!m_pTexture2D)
		{
			DV_CORE_ERROR("Texture2D가 존재하지 않아 데이터를 설정할 수 없습니다.");
			return false;
		}

		if(level >= m_MipLevels)
		{
			DV_CORE_ERROR("잘못된 MipLevel을 전달받았습니다.");
			return false;
		}

		uint32_t levelWidth = GetLevelWidth(level);
		uint32_t levelHeight = GetLevelHeight(level);
		if (x < 0 || x + width  > levelWidth || y < 0 || y + height > levelHeight || width <= 0 || height <= 0)
		{
			DV_CORE_ERROR("잘못된 크기를 전달받았습니다.");
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
				DV_CORE_ERROR("Texture Map에 실패하였습니다.");
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
			DV_CORE_ERROR("Image를 잘못 전달 받았습니다.");
			return false;
		}

		// 밉맵이 생성된 상태를 가정한다.

		return true;
	}

	bool DvTexture2D::Create()
	{
		Release();

		if (!m_Width || !m_Height)
			return false;

		// MipLevels 계산
		// 1. 우선 width, height, requestedLevels?를 이용해 직접 계산한다.
		// 2. DSV를 만들면 1로 다시 고정한다.
		// 3. 마지막으로 multiSample == 1, !TEXTURE_DYNAMIC일 경우 앞선 값을, 아니라면 1로 고정한다.
		// TEXTURE_RENDERTARGET, level != 1, multiSample == 1 일 경우 D3D11_RESOURCE_MISC_GENERATE_MIPS를 설정한다.
		// GenerateMips() 함수가 랩핑되어 있다. rendertarget이 그려진 후, 샘플링 전에 호출된다는데...
		// rastertek의 경우 srv를 로드한 후 호출했다. 즉, srv를 사용하기 전 호출하는 듯 하다.

		// texture2d
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = (UINT)m_Width;
		texDesc.Height = (UINT)m_Height;
		texDesc.Format = m_Format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;			// 임시
		texDesc.SampleDesc.Count = 1;	// 임시
		texDesc.SampleDesc.Quality = 0;	// 임시
		texDesc.Usage = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_Usage == eTextureUsage::TEXTURE_RENDERTARGET)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else if (m_Usage == eTextureUsage::TEXTURE_DEPTHSTENCIL)
			texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		texDesc.MiscFlags = 0;	// 임시 generate mips가 될 수 있다.
		texDesc.CPUAccessFlags = m_Usage == eTextureUsage::TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_CORE_ERROR("Texture 생성에 실패하였습니다.");
			Release();
			return false;
		}

		// srv
		if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = GetSRVFormat(m_Format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	// 임시, multi sample이 될 수 있다.
			srvDesc.Texture2D.MipLevels = 1;		// 임시, -1이면 전체를 사용한다는 의미
			srvDesc.Texture2D.MostDetailedMip = 0;	// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateShaderResourceView((ID3D11Resource*)m_pTexture2D, &srvDesc, &m_pShaderResourceView)))
			{
				DV_CORE_ERROR("ShaderReosurceView 생성에 실패하였습니다.");
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
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// 임시, multi sample이 될 수 있다.
			rtvDesc.Texture2D.MipSlice = 0;		// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateRenderTargetView((ID3D11Resource*)m_pTexture2D, &rtvDesc, &m_pRenderTargetView)))
			{
				DV_CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
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
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// 임시, multi sample이 될 수 있다.
			dsvDesc.Texture2D.MipSlice = 0;	// 임시

			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilView)))
			{
				DV_CORE_ERROR("DepthStencilView 생성에 실패하였습니다.");
				Release();
				return false;
			}

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (FAILED(Renderer::GetGraphicsDevice().GetDevice()->CreateDepthStencilView((ID3D11Resource*)m_pTexture2D, &dsvDesc, &m_pDepthStencilViewReadOnly)))
			{
				DV_CORE_ERROR("DepthStencilViewReadOnly 생성에 실패하였습니다.");
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