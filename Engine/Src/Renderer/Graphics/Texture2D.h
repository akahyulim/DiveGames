#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
		RESOURCE_CLASS_TYPE(Texture2D)

	public:
		Texture2D();
		~Texture2D();

		void Release() override;

		// ���� �Ӹʰ� ��Ƽ���ø��� ���ܵƴ�.
		bool LoadFromFile(const std::string& filepath) override;

		// ���� �Ӹʰ� ��Ƽ���ø��� ������.
		// �ϴ� ��Ƽ���ø��� �����´� �ص� �Ӹ� ������ ���� ���θ� ��� �������� �����ؾ� �Ѵ�.
		bool SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::TEXTURE_STATIC);
		bool SetData(uint32_t level, int x, int y, uint32_t width, uint32_t height, const void* pData);
		
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