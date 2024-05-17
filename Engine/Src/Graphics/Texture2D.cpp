#include "divepch.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Resource/Image.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"

/*
* 파일 Load
* - Mipmap을 사용하지 않을 땐 ShaderResourceView만 생성
* - 아니라면 RenderTargetView도 함께 생성
* - UpdateSubresource()를 사용
* - 현재 MultiSample은 논외
* 
* Texture 직접 생성
* - ShaderResourceView와 RenderTargetView 둘 다 생성
* - Map/Unmap()을 사용 == Dynamic
* - 현재 Mipmap과 MultiSample은 논외
*/
namespace Dive
{
	Texture2D* Texture2D::s_pBlackTexture = nullptr;
	Texture2D* Texture2D::s_pGrayTexture = nullptr;
	Texture2D* Texture2D::s_pRedTexture = nullptr;
	Texture2D* Texture2D::s_pWhiteTexture = nullptr;

	Texture2D::Texture2D()
		: m_pRenderTargetView(nullptr),
		m_pLoadImage(nullptr)
	{
	}

	Texture2D::~Texture2D()
	{
		DV_DELETE(m_pLoadImage);
		DV_RELEASE(m_pRenderTargetView);

		DV_ENGINE_TRACE("resource destroy - {0:s}({1:d}), {2:s}({3:d})",
			GetTypeName(), GetTypeHash(), GetName(), GetNameHash());
	}

	// 현재 m_pLoadImage는 Cache에 등록되지 않고
	// 이 클래스 내부에서 관리한다.
	bool Texture2D::LoadFromFile(const std::string& filePath)
	{
		m_pLoadImage = new Image();
		if (!m_pLoadImage->LoadFromFile(filePath))
		{
			DV_DELETE(m_pLoadImage);
			return false;
		}

		// 추후 smapler, mimap, sRGB 설정을 로드한 후 적용하는
		// SetParameters() 필요
	
		if (!SetImage(m_pLoadImage))
			return false;

		// 왜 또 굳이 지웠냐...?
		// 그럼 동적생성한 의미가 없잖아...
		DV_DELETE(m_pLoadImage);

		//m_FilePath = filePath;
		SetName(FileSystem::GetFileName(filePath));

		return true;
	}

	// 이미지 파일을 이용해 텍스쳐 생성
	// parameter 파일을 통해 smapler, mipmap 여부를 추가로 설정
	// mipmap을 사용하지 않는다면 srv만으로 충분
	bool Texture2D::SetImage(Image* pImage)
	{
		if (!pImage)
		{
			DV_ENGINE_ERROR("Texture2D::SetImage - 잘못된 Image를 전달받았습니다.");
			return false;
		}

		m_Width = pImage->GetWidth();
		m_Height = pImage->GetHeight();
		m_Format = pImage->GetFormat();
		m_Usage = eTextureUsage::Static;
		m_bOpaque = pImage->IsOpaque();
		
		// SetParameters에서 설정되어야 하는 항목들
		m_bLinear = true;
		//m_bLinear = m_Format != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// Mipmap의 경우 0 or 1로 받으며
		// 0이면 최대, 1이면 사용하지 않음을 뜻하게 된다.
		// 이는 m_RequestedMipLevels에 저장한다.
		m_RequestedMipLevels = 0;
		m_MipLevels = CheckMaxMipLevels(pImage->GetWidth(), pImage->GetHeight(), m_RequestedMipLevels);


		if (!createResources())
			return false;

		if (!SetRawTextureData(static_cast<const void*>(pImage->GetPixels())))
			return false;

		return true;
	}

	// cpu에서 쓸 수 있는 텍스쳐 생성
	// 기본적으로 rtv를 만들지 않아도 되지만
	// mipmap을 사용하려면 만들어야 할 것 같다.
	// 현재는 안만들도록 했다.
	bool Texture2D::SetSize(int width, int height, DXGI_FORMAT format, int mipCount, bool linear)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bLinear = linear;
		m_Usage = eTextureUsage::Dynamic;

		// 기본적으로 하나만 설정되어야 dynamic이 가능하다.
		// device 생성과정에서 변경할 수 있다는데 추후 다시 알아보자.
		m_MipLevels = 1;

		return createResources();
	}

	// 역시 밉맵레벨은 무시
	void Texture2D::SetPixels(const std::vector<DirectX::XMINT4>& colors, int mipLevel)
	{
		if (m_Usage != eTextureUsage::Dynamic || !m_pTexture2D)
			return;
		
		if (m_Width * m_Height != static_cast<int>(colors.size()))
			return;

		// mipmap을 적용하려면 
		// 추후 D3D11CalcSubresource()를 사용하여 변경
		unsigned int subResource = 0;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		
		if (FAILED(m_pDeviceContext->Map(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			subResource,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource)))
		{
			DV_ENGINE_ERROR("Texture2D::SetRawTextureData - 텍스쳐 맵에 실패하였습니다.");
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

		m_pDeviceContext->Unmap(static_cast<ID3D11Resource*>(m_pTexture2D), subResource);
	}

	bool Texture2D::SetRawTextureData(const void* pData)
	{
		if (!m_pTexture2D)
		{
			DV_ENGINE_ERROR("Texture2D::SetRawTextureData - 텍스쳐가 존재하지 않아 데이터를 전달할 수 없습니다.");
			return false;
		}

		if (!pData)
		{
			DV_ENGINE_ERROR("Texture2D::SetRawTextureData - 비어있는 데이터를 전달받았습니다.");
			return false;
		}

		const void* pSrc = pData;
		unsigned int rowPitch = GetRowPitchSize(m_Width);

		m_pDeviceContext->UpdateSubresource(
			m_pTexture2D,
			0,
			nullptr,
			pSrc,
			rowPitch,
			0);

		return true;
	}

	/*
	Texture2D* Texture2D::GetBlackTexture()
	{
		DV_ENGINE_ASSERT(Graphics::GetInstance()->IsInitialized());

		if (!s_pBlackTexture)
		{
			DirectX::XMINT2 size = Graphics::GetInstance()->GetWindowSize();

			s_pBlackTexture = new Texture2D();
			s_pBlackTexture->SetSize(size.x, size.y);
			
			std::vector<DirectX::XMINT4> colors;
			colors.assign((size_t)(size.x * size.y), DirectX::XMINT4(0, 0, 0, 0));

			s_pBlackTexture->SetPixels(colors);
		}

		return s_pBlackTexture;
	}

	Texture2D* Texture2D::GetGrayTexture()
	{
		DV_ENGINE_ASSERT(Graphics::GetInstance()->IsInitialized());

		if (!s_pGrayTexture)
		{
			DirectX::XMINT2 size = Graphics::GetInstance()->GetWindowSize();

			s_pGrayTexture = new Texture2D();
			s_pGrayTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign((size_t)(size.x * size.y), DirectX::XMINT4(128, 128, 128, 128));

			s_pGrayTexture->SetPixels(colors);
		}

		return s_pGrayTexture;
	}

	Texture2D* Texture2D::GetRedTexture()
	{
		DV_ENGINE_ASSERT(Graphics::GetInstance()->IsInitialized());

		if (!s_pRedTexture)
		{
			DirectX::XMINT2 size = Graphics::GetInstance()->GetWindowSize();

			s_pRedTexture = new Texture2D();
			s_pRedTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign((size_t)(size.x * size.y), DirectX::XMINT4(255, 0, 0, 0));

			s_pRedTexture->SetPixels(colors);
		}

		return s_pRedTexture;
	}

	Texture2D* Texture2D::GetWhiteTexture()
	{
		DV_ENGINE_ASSERT(Graphics::GetInstance()->IsInitialized());

		if (!s_pWhiteTexture)
		{
			DirectX::XMINT2 size = Graphics::GetInstance()->GetWindowSize();

			s_pWhiteTexture = new Texture2D();
			s_pWhiteTexture->SetSize(size.x, size.y);

			std::vector<DirectX::XMINT4> colors;
			colors.assign((size_t)(size.x * size.y), DirectX::XMINT4(255, 255, 255, 255));

			s_pWhiteTexture->SetPixels(colors);
		}

		return s_pWhiteTexture;
	}
	*/
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
		texDesc.SampleDesc.Count = 1;		// 현재 멀티 샘플링 미지원.
		texDesc.SampleDesc.Quality = 0;		// 현재 멀티 샘플링 미지원.
		texDesc.Usage = m_Usage == eTextureUsage::Dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if(m_MipLevels > 1)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = m_MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		texDesc.CPUAccessFlags = m_Usage == eTextureUsage::Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			DV_RELEASE(m_pTexture2D);
			DV_ENGINE_ERROR("Texture2D::createResources - Texture2D 생성에 실패하였습니다.");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = GetSRVFormat(texDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// 멀티 샘플링 미지원.
		srvDesc.Texture2D.MipLevels = -1;// m_MipLevels;// m_Usage == eTextureUsage::Dynamic ? m_MipLevels : 1;
		//srvDesc.DTexture2D.MipLevels = m_bDynamic ? 1 : -1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			DV_RELEASE(m_pShaderResourceView);
			DV_ENGINE_ERROR("Texture2D::createResources - ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		if(texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = texDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// 멀티 샘플링 미지원.
			rtvDesc.Texture2D.MipSlice = 0;

			if (FAILED(m_pDevice->CreateRenderTargetView(
				static_cast<ID3D11Resource*>(m_pTexture2D),
				&rtvDesc,
				&m_pRenderTargetView)))
			{
				DV_RELEASE(m_pRenderTargetView);
				DV_ENGINE_ERROR("Texture2D::createResources - RenderTargetView 생성에 실패하였습니다.");
				return false;
			}
		}
		
		return true;
	}

	Texture2D* LoadTexture2dFromFile(const std::string& fileName)
	{
		Texture2D* pObject = new Texture2D();
		if (!pObject->LoadFromFile(fileName))
		{
			DV_DELETE(pObject);
			return nullptr;
		}

		pObject->SetName(fileName);

		return pObject;
	}
}