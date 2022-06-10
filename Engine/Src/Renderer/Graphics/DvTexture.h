#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	// 추후 GraphicsDefs로 이동
	enum class eTextureUsage
	{
		TEXTURE_STATIC = 0,
		TEXTURE_DYNAMIC,
		TEXTURE_RENDERTARGET,
		TEXTURE_DEPTHSTENCIL
	};

	class DvTexture : public Resource
	{
		RESOURCE_CLASS_TYPE(Texture)

	public:
		DvTexture();
		virtual ~DvTexture() = default;

		// 기존의 Destroy or Shutdown. 이쪽 이름이 더 마음에 든다.
		// urho의 경우 GPUObject에 선언했다.
		virtual void Release() {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetArraySize() const { return m_ArraySize; }
		uint32_t GetMipLevels() const { return m_MipLevels; }
		
		// SRGB는 뭘까?
		DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format);
		DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

		void RegenerateMips();

	protected:
		virtual bool Create() { return true; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ArraySize = 1;
		uint32_t m_MipLevels = 1;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		eTextureUsage m_Usage = eTextureUsage::TEXTURE_STATIC;
		
		ID3D11Texture2D* m_pTexture2D = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		// 배열일 수 있다. Texture2D의 경우 하나씩만 사용한다면
		// Texture2D, Textur2DArray, TextureCube 마다 달리 선언하는 편이 낫다.
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly = nullptr;
	};
}