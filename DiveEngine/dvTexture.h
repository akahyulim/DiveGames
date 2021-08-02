#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include <string>

namespace dive
{
	// 아... Resource를 상속해야 하는데.. 아직 구현을 안했다.
	class dvTexture
	{
	public:
		dvTexture(ID3D11Device* pDevice);

		bool LoadFromFile(const std::string& filepath, bool generateMips = true);

		// get & set

	private:
	private:
		ID3D11Device* m_pDevice = nullptr;

		ID3D11Texture2D* m_pTexture = nullptr;
		ID3D11DepthStencilView* m_pDSV = nullptr;
		ID3D11RenderTargetView* m_pRTV = nullptr;
		ID3D11ShaderResourceView* m_pSRV = nullptr;
	};
}