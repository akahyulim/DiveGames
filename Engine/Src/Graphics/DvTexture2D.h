#pragma once
#include "DvTexture.h"

namespace Dive
{
	class Image;

	// �⺻�����δ� Load�� ���� srv ����
	// �߰��� Ư�� ũ���� �ؽ��ĸ� ����� ���� ������ set, get -> rtv
	class DvTexture2D : public DvTexture
	{
	public:
		DvTexture2D();
		~DvTexture2D() override;

		bool LoadFromFile(const std::string& filePath) override;

		// urho3d�� �⺻������ SetSize�� SetData �ѷ� ������.
		// SetSize�� �ƹ����� RenderTargetView���ε� �ϰ�
		// SetData�� ShaderResourceView���ε� �ϴ�. 
		// => �ƴϴ�. SetSize�� ������ �ؽ��ĸ� �̿��ؼ� ���𰡸� �Ѵ�.
		// => ���� SetRawTextureData�� ����.

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