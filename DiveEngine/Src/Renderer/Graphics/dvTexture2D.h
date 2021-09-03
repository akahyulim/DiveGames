#pragma once
#include "dvTexture.h"
#include "../Renderer.h"
#include <string>
#include <DirectXMath.h>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		// map / unmap도 생각해야 하나?

		dvTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipMap = false)
			: dvTexture(typeid(dvTexture2D).hash_code())
		{
			m_pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			m_pDeviceContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

			m_Width = width;
			m_Height = height;
			m_Format = format;
			m_bMipmaps = mipMap;
			m_MipmapCount = m_bMipmaps ? 0 : 1;

			createResource();
		}

		// override
		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

		// 유니티에서도 생성자와 매우 비슷하다고 언급한다. 존재하는 텍스쳐 객체를 다루는 점만 다르단다.
		// 그런데 생각해보니 크기를 변경할 수 없다. 따로 알고리즘이나 함수가 필요할 것 같다.
		// 아니 애초에 Mipmap이 존재하면 필요도 없는 행위다.
		bool Resize(unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipMap = false);

		void Apply();
		bool LoadData(const std::vector<std::byte>& data);

		// get
		DXGI_FORMAT GetFormat() const { return m_Format; }
		unsigned int GetMipmapCount() const { return m_MipmapCount; }

		ID3D11Texture2D* GetResource() { return m_pResource.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); }

	private:
		bool createResource();

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		bool m_bMipmaps = false;
		unsigned int m_MipmapCount = 0;
		std::vector<std::byte> m_data;
	};
}