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

		// RenderTexture의 Create와 동일하게 만들자.
		// 아니 아에 이 둘을 Texture에서 순수가상함수로 선언하는 편이 나을 듯?
		bool CreateResource(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useMipMap);
		void Release();

		bool UpdateSubresource(const void* pData, uint32_t rowPitch);

		ID3D11ShaderResourceView* GetShaderResourceView() override { return m_pShaderResourceView; }

	private:
		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
	};
}