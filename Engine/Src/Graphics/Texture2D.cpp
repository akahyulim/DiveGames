#include "divepch.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"

/*
* LoadFromFile의 경우 밉맵을 자동 생성
* 색 공간을 고려하지 않음
* 멀티 샘플링 미지원
*/
namespace Dive
{	
	Texture2D::Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipMap)
		: m_pTexture2D(nullptr)
		, m_pShaderResourceView(nullptr)
	{
		CreateResource(width, height, format, useMipMap);
	}

	Texture2D::~Texture2D()
	{
		Release();
	}
	
	bool Texture2D::LoadFromFile(const std::string& filename)
	{
		auto file = StringToWstring(filename);
		auto ext = FileSystem::GetExtension(filename);
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		if (ext == ".dds")
		{
			hResult = DirectX::LoadFromDDSFile(file.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (ext == ".tga")
		{
			// 색 공간 정보를 포함하지 않는다.
			hResult = DirectX::LoadFromTGAFile(file.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(file.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_ENGINE_ERROR("파일 {:s} 로드에 실패하여 Texture2D를 생성이 중단되었습니다.", filename);
			return false;
		}

		if (!CreateResource(
			static_cast<uint32_t>(img.GetImages()->width),
			static_cast<uint32_t>(img.GetImages()->height),
			img.GetImages()->format, true))
			return false;

		if (!UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch)))
			return false;

		m_bOpaque = img.IsAlphaAllOpaque();
		SetName(FileSystem::GetFileName(filename));

		return true;
	}

	bool Texture2D::LoadFromMemory(const std::string& filename, size_t size, const void* pSource)
	{
		DirectX::ScratchImage img;
		HRESULT hResult = 0;

		auto extension = FileSystem::GetExtension(filename);

		if (extension == "dds")
		{
			hResult = DirectX::LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (extension == "tga")
		{
			hResult = DirectX::LoadFromTGAMemory(pSource, size, nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICMemory(pSource, size, DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hResult))
		{
			DV_ENGINE_ERROR("{:s} 형식의 메모리 로드에 실패하였습니다.", extension);
			return false;
		}

		if (!CreateResource(
			static_cast<uint32_t>(img.GetImages()->width),
			static_cast<uint32_t>(img.GetImages()->height),
			img.GetImages()->format, true))
			return false;

		if (!UpdateSubresource((const void*)img.GetImages()->pixels, static_cast<uint32_t>(img.GetImages()->rowPitch)))
			return false;

		m_bOpaque = img.IsAlphaAllOpaque();
		SetName(FileSystem::GetFileName(filename));

		return true;
	}
	
	bool Texture2D::CreateResource(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipMap)
	{
		Release();

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = format;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = m_bUseMipMap ? 0 : 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (useMipMap)
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = useMipMap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		texDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pTexture2D)))
		{
			Release();
			DV_ENGINE_ERROR("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(
			static_cast<ID3D11Resource*>(m_pTexture2D),
			&srvDesc,
			&m_pShaderResourceView)))
		{
			Release();
			DV_ENGINE_ERROR("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_bUseMipMap = useMipMap;

		return true;
	}
	
	void Texture2D::Release()
	{
		DV_RELEASE(m_pShaderResourceView);
		DV_RELEASE(m_pTexture2D);
	}

	bool Texture2D::UpdateSubresource(const void* pData, uint32_t rowPitch)
	{
		if (!m_pTexture2D)
		{
			DV_ENGINE_ERROR("ID3D11Texture2D 객체가 존재하지 않아 서브 리소스 업데이트에 실패하였습니다.");
			return false;
		}

		if (!pData)
		{
			DV_ENGINE_ERROR("비어있는 데이터를 전달받아 서브 리소스 업데이트에 실패하였습니다.");
			return false;
		}

		m_pDeviceContext->UpdateSubresource(
			m_pTexture2D,
			0,
			nullptr,
			pData,
			rowPitch,
			0);

		if (m_bUseMipMap)
			m_pDeviceContext->GenerateMips(m_pShaderResourceView);

		return true;
	}
}