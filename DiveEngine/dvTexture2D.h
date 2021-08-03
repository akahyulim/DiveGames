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

		// apply??? => set pixel �� ���� �����Ű�� ����� ū �۾��̶� �Ѵ�.

		// static black, white: �׷��� ����Ƽ���� ���� ������ ����. ũ�� �� ���� ������ �ʿ����ٵ�...

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