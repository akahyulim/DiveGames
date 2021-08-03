#pragma once
#include "Object.h"
#include "GraphicsDevice.h"
#include "DiveCore.h"

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
			m_pDevice = pDevice;
		}
		virtual ~dvTexture() = default;

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	protected:
		ID3D11Device* m_pDevice = nullptr;

		unsigned int m_Width	= 0;
		unsigned int m_Height	= 0;
		
		// ansioLevel
		D3D11_FILTER m_FilterMode					= D3D11_FILTER_COMPARISON_ANISOTROPIC;// filterMode
		D3D11_TEXTURE_ADDRESS_MODE m_AddressMode	= D3D11_TEXTURE_ADDRESS_WRAP;// wrapMode
		// mipMapBias
	};
}