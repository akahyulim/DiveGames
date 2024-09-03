#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
		DV_CLASS(Texture2D, Texture)

	public:
		Texture2D() = default;
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		~Texture2D() override;

		bool LoadFromFile(const std::string& filename) override;
		bool LoadFromMemory(const std::string& filename, size_t size, const void* pSource);

		bool CreateResource(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipMap);
		void Release();

		bool UpdateSubresource(const void* pData, uint32_t rowPitch);

		ID3D11ShaderResourceView* GetShaderResourceView() override { return m_pShaderResourceView; }

	private:
		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
	};
}