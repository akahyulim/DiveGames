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

		// 역시 밉맵과 멀티샘플링은 제외됐다.
		bool LoadFromFile(const std::string& filepath) override;

		// 역시 밉맵과 멀티샘플링이 문제다.
		// 일단 멀티샘플링을 빼놓는다 해도 밉맵 레벨과 생성 여부를 어떻게 전달할지 결정해야 한다.
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