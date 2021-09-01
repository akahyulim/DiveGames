#pragma once
#include "dvTexture.h"
#include <string>
#include <DirectXMath.h>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		dvTexture2D(unsigned int width, unsigned int height)
			: dvTexture(typeid(dvTexture2D).hash_code())
		{
			// 2의 배수 크기여야 한다.
			m_Width = width;
			m_Height = height;
			m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			m_bMipmaps = false;
			m_MipCount = 1;
			m_ArraySize = 1;

			createResource();
		}

		dvTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format, bool mipMap)
			: dvTexture(typeid(dvTexture2D).hash_code())
		{
			m_Width = width;
			m_Height = height;
			m_Format = format;
			m_bMipmaps = mipMap;
			// 이게 0이라는건 에바다. 결국 count는 크기에 맞춰 정해진다.
			m_MipCount = m_bMipmaps ? 0 : 1;

			m_ArraySize = 1;

			createResource();
		}

		bool Resize(unsigned int width, unsigned int height, DXGI_FORMAT format, bool mipMap);

		void Apply();
		bool LoadData(const std::vector<std::byte>& data);

		// get
		ID3D11Texture2D* GetResource() { return m_pResource.Get(); }
		ID3D11ShaderResourceView* GetSRV() { return m_pShaderResourceView.Get(); }

	private:
		bool createResource();

	protected:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;

		std::vector<std::byte> m_data;
	};
}