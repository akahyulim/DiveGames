#pragma once
#include "DvTexture.h"

namespace Dive
{
	class Image;

	class DvTexture2D : public DvTexture
	{
		RESOURCE_CLASS_TYPE(Texture2D)

	public:
		DvTexture2D();
		~DvTexture2D();

		void Release() override;

		// load: image도 만들고 텍스쳐도 만든다. mipmap은...?
		bool LoadFromFile(const std::string& filepath) override;

		// 역시 밉맵과 멀티샘플링이 문제다.
		// 일단 멀티샘플링을 빼놓는다 해도 밉맵 레벨과 생성 여부를 어떻게 전달할지 결정해야 한다.
		bool SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::TEXTURE_STATIC);
		bool SetData(uint32_t level, int x, int y, uint32_t width, uint32_t height, const void* pData);
		bool SetData(Image* pImage, bool useAlpha = false);

		// image
		// 1. Load에서 생성한 후 텍스쳐를 만들면 clear한다.
		// 2. bool GetImage(Image& image): 텍스쳐로부터 Image Data를 구성해 매개변수에 복사하여 전달한다.
		// 3. Image* GetImage(): Image를 동적생성한 후 위의 함수로 구성한 후 리턴한다.

		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() { return m_pDepthStencilViewReadOnly; }

		// temp
		D3D11_VIEWPORT* GetViewport() { return &m_Viewport; }

	protected:
		bool Create() override;

	private:
	private:
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly = nullptr;

		D3D11_VIEWPORT m_Viewport;	// temp
	};
}