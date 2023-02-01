#pragma once
#include "Texture.h"

namespace Dive
{
	class Image;

	// �⺻�����δ� Load�� ���� srv ����
	// �߰��� Ư�� ũ���� �ؽ��ĸ� ����� ���� ������ set, get -> rtv
	class Texture2D : public Texture
	{
		DIVE_OBJECT(Texture2D, Texture)

	public:
		explicit Texture2D(Context* pContext);
		~Texture2D() override;

		bool LoadFromFile(const std::string& fileName) override;

		bool SetImage(Image* pImage);
		bool SetSize(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, int mipCount = 0, bool linear = false);

		void SetPixel(int x, int y, const DirectX::XMINT4& color, int mipLevel = 0);
		void SetPixels(const std::vector<DirectX::XMINT4>& colors, int mipLevel = 0);

		bool SetRawTextureData(const uint8_t* pData);

		DirectX::XMINT4 GetPixel(int x, int y, int mipLevel = 0);
		DirectX::XMINT4 GetPixels(int mipLevel = 0);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }

		static Texture2D* GetBlackTexture(Context* pContext);
		static Texture2D* GetGrayTexture(Context* pContext);
		static Texture2D* GetRedTexture(Context* pContext);
		static Texture2D* GetWhiteTexture(Context* pContext);

		static void RegisterObject(Context* pContext);

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
}