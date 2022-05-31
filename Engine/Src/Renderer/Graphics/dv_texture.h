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

		// 공통 함수

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

	// 위키드의 경우 생성 자체는 GraphicsDevice한다.
	// 이때 매개변수로 desc, subresource data와 함께 Texture포인터를 전달하는데
	// Texture 포인터에 views가 들어있는 구조체와 desc를 저장한다.
	// 따라서 실제 생성과정은 'desc 구성 -> 생성 함수 호출 -> 필요하다면 이름 추가'로 진행된다.
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