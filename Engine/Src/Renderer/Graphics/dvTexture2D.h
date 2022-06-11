#pragma once
#include "DvTexture.h"

namespace Dive
{
	class DvTexture2D : public DvTexture
	{
		RESOURCE_CLASS_TYPE(Texture2D)

	public:
		DvTexture2D();
		~DvTexture2D();

		void Release() override;

		// 역시 밉맵과 멀티샘플링이 문제다.
		// 일단 멀티샘플링을 빼놓는다 해도 밉맵 레벨과 생성 여부를 어떻게 전달할지 결정해야 한다.
		bool SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::TEXTURE_STATIC);
		// SetData: Map 
		// SetData: Image => 결국 이게 Load를 대신하는 거다...
		// => 데이터를 로드한 후 SetSize로 만들고 다시 SetData로 재구성하는 듯 하다.

		// Image 역시 resource다.
		// Resource::Load<T>()로 파일을 읽는다.
		// ResourceCache(manager?)::GetResource() -> Texture2D::BeginLoad() -> Image::Load() 순으로 호출된다.
		// Texture2D::EndLoad()에서 SetData()에 생성된 Image를 전달하여 구성된다.
		// BeginLoad(), EndLoad()는 Resource::Load() 내부에서 순차적으로 실행된다.

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