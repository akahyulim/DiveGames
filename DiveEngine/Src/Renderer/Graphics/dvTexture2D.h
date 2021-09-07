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

		dvTexture2D()
			: dvTexture(typeid(dvTexture2D).hash_code())
		{
			m_pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			m_pDeviceContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		}

		dvTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool generateMips = false)
			: dvTexture(typeid(dvTexture2D).hash_code())
		{
			m_pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			m_pDeviceContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

			m_Width = width;
			m_Height = height;
			m_Format = format;
			m_bMipmaps = generateMips;
			m_MipLevels = m_bMipmaps ? 0 : 1;

			createResource();
		}

		bool LoadFromFile(const std::string& filepath, bool generateMips = false);

		void Apply();
		bool LoadData(const std::vector<std::byte>& data);
		bool LoadData(const BYTE* pData);

		// get
		DXGI_FORMAT GetFormat() const { return m_Format; }
		unsigned int GetMipLevels() const { return m_MipLevels; }

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
		unsigned int m_MipLevels = 0;
		std::vector<std::byte> m_Data;
	};
}