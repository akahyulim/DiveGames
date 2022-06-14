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

		// load: image�� ����� �ؽ��ĵ� �����. mipmap��...?
		bool LoadFromFile(const std::string& filepath) override;

		// ���� �Ӹʰ� ��Ƽ���ø��� ������.
		// �ϴ� ��Ƽ���ø��� �����´� �ص� �Ӹ� ������ ���� ���θ� ��� �������� �����ؾ� �Ѵ�.
		bool SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::TEXTURE_STATIC);
		bool SetData(uint32_t level, int x, int y, uint32_t width, uint32_t height, const void* pData);
		bool SetData(Image* pImage, bool useAlpha = false);

		// image
		// 1. Load���� ������ �� �ؽ��ĸ� ����� clear�Ѵ�.
		// 2. bool GetImage(Image& image): �ؽ��ķκ��� Image Data�� ������ �Ű������� �����Ͽ� �����Ѵ�.
		// 3. Image* GetImage(): Image�� ���������� �� ���� �Լ��� ������ �� �����Ѵ�.

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