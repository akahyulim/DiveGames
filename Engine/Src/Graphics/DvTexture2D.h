#pragma once
#include "DvTexture.h"

namespace Dive
{
	class Image;

	// 기본적으로는 Load를 통해 srv 구성
	// 추가로 특정 크기의 텍스쳐를 만들어 놓고 색상을 set, get -> rtv
	class DvTexture2D : public DvTexture
	{
	public:
		DvTexture2D();
		~DvTexture2D() override;

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

		//static DvTexture2D* GetBlackTexture();
		//static DvTexture2D* GetGrayTexture();
		//static DvTexture2D* GetRedTexture();
		//static DvTexture2D* GetWhiteTexture();

	private:
		bool createResources() override;

	private:
		ID3D11RenderTargetView* m_pRenderTargetView;

		Image* m_pLoadImage;

		static DvTexture2D* s_pBlackTexture;
		static DvTexture2D* s_pGrayTexture;
		static DvTexture2D* s_pRedTexture;
		static DvTexture2D* s_pWhiteTexture;
	};
}