#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	struct MipmapData
	{
		std::vector<std::byte> bytes;
	};

	struct TextureSliceData
	{
		std::vector<MipmapData> mips;
		unsigned int GetMipCount() const { static_cast<unsigned int>(mips.size()); }
	};

	class DV_Texture// : public Resource
	{
	public:
		//RESOURCE_CLASS_TYPE(Texture)

		DV_Texture();
		virtual ~DV_Texture();

		// get / set

		// ���� �Լ�

	protected:
		bool createTexture();
		bool createShaderResourceView(int firstSlice, unsigned int sliceCount, unsigned int mipCount, unsigned int firstMip);
		bool createRenderTargetView(int firstSlice, unsigned int sliceCount, unsigned int mipCount, unsigned int firstMip);
		bool createDepthStencilView();

	protected:
		unsigned int m_Width			= 0;
		unsigned int m_Height			= 0;
		unsigned int m_MipLevels		= 1;
		unsigned int m_ArraySize		= 1;
		unsigned int m_BindFlags		= 0;
		unsigned int m_MiscFlags		= 0;
		unsigned int m_ChannelCount		= 0;
		unsigned int m_BitsPerChannel	= 0;
		
		std::vector<TextureSliceData> m_Data;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		D3D11_VIEWPORT m_Viewport;

		ID3D11Texture2D* m_pTexture2D = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pReadOnlyDepthStencilView = nullptr;
	};

	// ��Ű���� ��� ���� ��ü�� GraphicsDevice�Ѵ�.
	// �̶� �Ű������� desc, subresource data�� �Բ� Texture�����͸� �����ϴµ�
	// Texture �����Ϳ� views�� ����ִ� ����ü�� desc�� �����Ѵ�.
	// ���� ���� ���������� 'desc ���� -> ���� �Լ� ȣ�� -> �ʿ��ϴٸ� �̸� �߰�'�� ����ȴ�.
	class DV_Texture2D : public DV_Texture
	{
	public:
		// load: filepath
		DV_Texture2D(const std::string& filepath);
		DV_Texture2D(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int bindFlags, unsigned int miscFlags);
		

	private:
	private:
	};

	class DV_TextureCube : public DV_Texture
	{
	public:
		// srv + rtv
		// dsv
		// srv + rtv + dsv
		// load

	private:
	private:
	};
}