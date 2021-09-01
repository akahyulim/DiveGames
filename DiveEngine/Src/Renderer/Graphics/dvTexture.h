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
		dvTexture(size_t typeHash)
			: Object(typeHash)
		{
		}
		virtual ~dvTexture() = default;

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	protected:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		unsigned int m_Width	= 0;
		unsigned int m_Height	= 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		unsigned int m_ArraySize = 0;
		bool m_bMipmaps = false;
		unsigned int m_MipCount = 0;
		unsigned int m_BindFlags = 0;
		
		// ansioLevel
		// mipMapBias
	};
}