#pragma once
#include "GraphicsInclude.h"
#include "DirectXTex/DirectXTex.h"
#include <string>
#include <vector>

namespace Dive
{
	// 1. 일단 2D Texture 전용
	// 2. DepthStencilView는 미구현(Deferred에서 다시 해보자.)
	// 3. 파일 관련 Helper 함수 필요

	class Texture
	{
	public:
		Texture(ID3D11Device* pDevice);

		// cpu에서 그리기용(SRV)
		Texture(unsigned int width, unsigned int height);

		// SRV(파일명, mipmap 생성 여부) => 기본 생성자로 변경하기
		// 그리고 LoadFromFile()로 대체
		Texture(const std::wstring& filepath, bool generateMips = true);

		// RTV or DSV(size, view type, format type?)
		Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");

		bool LoadFromFile(const std::wstring& filepath, bool generateMips = false);

		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); }

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
		unsigned int GetMipLevls() const { return m_MipLevels; }
		unsigned int GetArraySize() const { return m_ArraySize; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		std::string GetName() const { return m_Name; }

		void SetWidth(unsigned int width) { m_Width = width; }
		void SetHeight(unsigned int height) { m_Height = height; }
		void SetFormat(DXGI_FORMAT format) { m_Format = format; }

		void SetChannelCount(unsigned int count) { m_ChannelCount = count; }
		void SetBitsPerChannel(unsigned int value) { m_BitsPerChannel = value; }
		void SetPitch(unsigned int pitch) { m_Pitch = pitch; }
		void SetData(std::vector<std::byte>& data) { m_Data = data; }


	private:
		bool createTexture2D(ID3D11Device* pDevice, unsigned flags);
		bool createShaderResourceView(ID3D11Device* pDevice);
		bool createRenderTargetView(ID3D11Device* pDevice);
		bool createDepthStencilView(ID3D11Device* pDevice);

		void setMetaData(const DirectX::TexMetadata& data);

	private:
		ID3D11Device* m_pDevice = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		
		std::string m_Name = "";

		// 최소한의 데이터 필요
		// 필요없는 건 지우자.
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		unsigned int m_MipLevels = 0;
		unsigned int m_ArraySize = 0;
		unsigned int m_ChannelCount = 0;
		unsigned int m_BitsPerChannel = 0;
		unsigned int m_Pitch = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		std::vector<std::byte> m_Data;
	};
}