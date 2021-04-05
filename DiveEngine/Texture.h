#pragma once
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	class Texture
	{
	public:
		// 생성 용도가 다양하다.
		// 일단 2D 데이터를 로드하여 ShaderResourceView를 생성토록 하자.
		Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::vector<std::vector<std::byte>>& rawData);


		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRTV.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pSRV.Get(); }

	private:

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

		std::string m_name;

		// 최소한의 데이터 필요
		unsigned int m_width;
		unsigned int m_height;
		DXGI_FORMAT m_format;
		std::vector<std::vector<std::byte>> m_rawData;
	};
}