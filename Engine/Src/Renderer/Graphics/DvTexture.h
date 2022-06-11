#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	// ���� GraphicsDefs�� �̵�
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

		// ������ Destroy or Shutdown. ���� �̸��� �� ������ ���.
		// urho�� ��� GPUObject�� �����ߴ�.
		virtual void Release() {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetArraySize() const { return m_ArraySize; }
		uint32_t GetMipLevels() const { return m_MipLevels; }
	
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

		void RegenerateMips();

		// SRGB�� ����?
		static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);

		static uint32_t CalMipMaxLevel(uint32_t width, uint32_t height);

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
	};
}