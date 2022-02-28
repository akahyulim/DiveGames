#include "divepch.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	// static varialbes
	GraphicsDevice Renderer::m_GraphicsDevice;

	unsigned int Renderer::m_TextureWidth = 0;
	unsigned int Renderer::m_TextureHeight = 0;

	Texture2D* Renderer::m_pSampleTex = nullptr;
	Texture2D* Renderer::m_pDepthStencilTex = nullptr;

	ID3D11DepthStencilState* Renderer::m_pDepthStencilState = nullptr;

	void Renderer::Initialize(const WindowData* pData)
	{
		DV_ASSERT(pData && "WnidowData가 존재하지 않습니다.");

		m_GraphicsDevice.Initialize(pData);

		auto width = pData->Width;
		auto height = pData->Height;

		SetTextures(width, height);
	}

	void Renderer::Shutdown()
	{
		// 함수로 만들어야 하나...?
		DV_DELETE(m_pDepthStencilTex);
		DV_DELETE(m_pSampleTex);

		m_GraphicsDevice.Shutdown();
	}
	
	// Camera를 받아 Data(viewport)를 저장
	// clear, set은 다른 곳에서?
	// Hazle은 오버로딩 되어있다.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pSampleTex ? m_pSampleTex->GetRenderTargetView() : nullptr;
		auto pDepthStencilView = m_pDepthStencilTex ? m_pDepthStencilTex->GetDepthStencilView() : nullptr;
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera로부터 가져와야 한다.
		float clearColors[4] = {0.5f, 0.5f, 1.0f, 1.0f};

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
	
	void Renderer::EndScene()
	{
		auto pSwapChain = m_GraphicsDevice.GetSwapChain();
		if (!pSwapChain)
			return;

		pSwapChain->Present(m_GraphicsDevice.IsVSync() ? 1 : 0, 0);
	}

	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		m_GraphicsDevice.ResizeBackBuffer(width, height);
	}

	void Renderer::SetTextures(unsigned int width, unsigned int height)
	{
		if (m_TextureWidth == width && m_TextureHeight == height)
			return;

		m_TextureWidth = width;
		m_TextureHeight = height;

		// Render Target Textures
		DV_DELETE(m_pSampleTex);
		m_pSampleTex = Texture2D::Create(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

		// Depth Stencil Buffers
		DV_DELETE(m_pDepthStencilTex);
		m_pDepthStencilTex = Texture2D::Create(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT);
	}

	ID3D11RenderTargetView* Renderer::GetMainRenderTargetView()
	{
		return m_GraphicsDevice.GetMainRenderTargetView();
	}
}