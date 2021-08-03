#pragma once
#include "dvTexture.h"
#include "DirectXTex/DirectXTex.h"
#include <string>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		dvTexture2D(ID3D11Device* pDevice, unsigned int width, unsigned int height);
		dvTexture2D(ID3D11Device* pDevice, const std::string& filepath, bool hasMipMap = false);
		~dvTexture2D();

		// get, set pixel

		// resize???

		// apply??? => set pixel 한 것을 적용시키는 비용이 큰 작업이라 한다.

		// static black, white: 그런데 유니티에서 생성 예제가 없다. 크기 및 포멧 설정이 필요할텐데...

		DXGI_FORMAT GetFormat() const { return m_Format; }
		unsigned int GetMipmapCount() const { return m_MipmapCount; }

		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pSRV; }

	private:
		void setMetadata(const DirectX::TexMetadata& data);

	private:
		DXGI_FORMAT m_Format				= DXGI_FORMAT_UNKNOWN;
		unsigned int m_MipmapCount			= 1;

		ID3D11Texture2D* m_pTexture			= nullptr;
		ID3D11ShaderResourceView* m_pSRV	= nullptr;

	};
}