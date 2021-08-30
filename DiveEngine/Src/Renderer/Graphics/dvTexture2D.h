#pragma once
#include "dvTexture.h"
#include "DirectXTex/DirectXTex.h"
#include <string>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		// render target or depth stencil view
		// 스파르탄은 이때 밉맵 생성을 지원하지 않았다.
		dvTexture2D(ID3D11Device* pDevice, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int bindFlags, unsigned int arraySize = 1)
			: dvTexture(typeid(dvTexture2D).hash_code(), pDevice)
		{
			// 각종 설정 초기화
			m_Width = width;
			m_Height = height;
			m_Format = format;
			m_ArraySize = arraySize;
			m_MipCount = 1;
			m_BindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
			
			createResourceAndViews(pDevice);
		}
		

		// shader reosurce view
		// load or 특정 색상으로 초기화

	private:
		void createResourceAndViews(ID3D11Device* pDevice);

	private:
	};
}