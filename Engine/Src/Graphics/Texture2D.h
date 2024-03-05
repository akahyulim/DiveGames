#pragma once
#include "Texture.h"

namespace Dive
{
	class Image;

	// 기본적으로는 Load를 통해 srv 구성
	// 추가로 특정 크기의 텍스쳐를 만들어 놓고 색상을 set, get -> rtv
	class Texture2D : public Texture
	{
		DV_OBJECT(Texture2D, Texture);

	public:
		Texture2D();
		~Texture2D() override;

		bool LoadFromFile(const std::string& filePath) override;

		// urho3d는 기본적으로 SetSize와 SetData 둘로 나뉜다.
		// SetSize는 아무래도 RenderTargetView용인듯 하고
		// SetData는 ShaderResourceView용인듯 하다. 
		// => 아니다. SetSize로 생성된 텍스쳐를 이용해서 무언가를 한다.
		// => 현재 SetRawTextureData와 같다.

		bool SetImage(Image* pImage);
		bool SetSize(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, int mipCount = 0, bool linear = false);

		void SetPixel(int x, int y, const DirectX::XMINT4& color, int mipLevel = 0);
		void SetPixels(const std::vector<DirectX::XMINT4>& colors, int mipLevel = 0);

		bool SetRawTextureData(const void* pData);

		DirectX::XMINT4 GetPixel(int x, int y, int mipLevel = 0);
		DirectX::XMINT4 GetPixels(int mipLevel = 0);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }

		//static Texture2D* GetBlackTexture();
		//static Texture2D* GetGrayTexture();
		//static Texture2D* GetRedTexture();
		//static Texture2D* GetWhiteTexture();

	private:
		bool createResources() override;

	private:
		ID3D11RenderTargetView* m_pRenderTargetView;

		Image* m_pLoadImage;

		static Texture2D* s_pBlackTexture;
		static Texture2D* s_pGrayTexture;
		static Texture2D* s_pRedTexture;
		static Texture2D* s_pWhiteTexture;
	};

	Texture2D* LoadTexture2dFromFile(const std::string& fileName);
}