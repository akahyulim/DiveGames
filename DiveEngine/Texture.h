#pragma once
#include "GraphicsInclude.h"
#include "DirectXTex/DirectXTex.h"
#include <string>
#include <vector>

namespace Dive
{
	// 일단 2D Texture 전용
	class Texture
	{
	public:
		// SRV(파일명, mipmap 생성 여부)
		Texture(const std::wstring& filepath, bool generateMips = true);

		// RTV or DSV(size, view type, format type?)
		Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");


		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRTV.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pSRV.Get(); }

		// get & set
		unsigned int GetWidth() const { return m_width; }
		unsigned int GetHeight() const { return m_height; }
		unsigned int GetMipLevls() const { return m_mipLevels; }
		unsigned int GetArraySize() const { return m_arraySize; }
		DXGI_FORMAT GetFormat() const { return m_format; }
		std::string GetName() const { return m_name; }

		void SetWidth(unsigned int width) { m_width = width; }
		void SetHeight(unsigned int height) { m_height = height; }
		void SetFormat(DXGI_FORMAT format) { m_format = format; }

		void SetChannelCount(unsigned int count) { m_channelCount = count; }
		void SetBitsPerChannel(unsigned int value) { m_bitsPerChannel = value; }
		void SetPitch(unsigned int pitch) { m_pitch = pitch; }
		void SetData(std::vector<std::byte>& data) { m_data = data; }


	private:
		bool createTexture2D(ID3D11Device* pDev, unsigned flags);
		bool createShaderResourceView(ID3D11Device* pDev);
		bool createRenderTargetView(ID3D11Device* pDev);
		bool createDepthStencilView(ID3D11Device* pDev);

		void setMetaData(const DirectX::TexMetadata& data);

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

		std::string m_name;

		// 최소한의 데이터 필요
		// 필요없는 건 지우자.
		unsigned int m_width = 0;
		unsigned int m_height = 0;
		unsigned int m_mipLevels = 0;
		unsigned int m_arraySize = 0;
		unsigned int m_channelCount = 0;
		unsigned int m_bitsPerChannel = 0;
		unsigned int m_pitch = 0;
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		std::vector<std::byte> m_data;
	};
}