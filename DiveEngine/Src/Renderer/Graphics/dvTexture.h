#pragma once
#include "../Object.h"
#include "../DiveCore.h"
#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace dive
{
	// 일단 Object 상속으로 가자. Resource의 필요성을 모르겠다.
	class dvTexture : public Object
	{
	public:
		dvTexture(size_t typeHash, ID3D11Device* pDevice)
			: Object(typeHash)
		{
			DV_ASSERT(pDevice != nullptr);
		}
		virtual ~dvTexture() = default;

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	protected:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

		unsigned int m_Width	= 0;
		unsigned int m_Height	= 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		unsigned int m_ArraySize = 0;
		unsigned int m_MipCount = 0;
		unsigned int m_BindFlags = 0;
		
		// ansioLevel
		// mipMapBias
	};
}