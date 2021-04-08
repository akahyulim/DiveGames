#pragma once
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	// 일단 2d texture, 단일, 밉맵 없이
	class Texture
	{
	public:
		
		bool LoadFromFile(const std::wstring& filepath);

		bool CreateGPUResource();

		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRTV.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pSRV.Get(); }

		// get & set
		void SetWidth(unsigned int width) { m_width = width; }
		void SetHeight(unsigned int height) { m_height = height; }
		void SetChannelCount(unsigned int count) { m_channelCount = count; }
		void SetBitsPerChannel(unsigned int value) { m_bitsPerChannel = value; }
		void SetPitch(unsigned int pitch) { m_pitch = pitch; }
		void SetFormat(DXGI_FORMAT format) { m_format = format; }
		void SetData(std::vector<std::byte>& data) { m_data = data; }

	private:

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

		std::string m_name;

		// 최소한의 데이터 필요
		unsigned int m_width = 0;
		unsigned int m_height = 0;
		unsigned int m_channelCount = 0;
		unsigned int m_bitsPerChannel = 0;
		unsigned int m_pitch = 0;
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		std::vector<std::byte> m_data;
	};
}