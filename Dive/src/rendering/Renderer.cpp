#include "stdafx.h"
#include "Renderer.h"
//#include "GBuffer.h"
#include "core/CoreDefs.h"
#include "graphics/Graphics.h"
#include "graphics/Texture2D.h"

namespace Dive
{
	Renderer::Renderer()
		: m_pGBuffer(std::make_unique<GBuffer>())
		, m_Width(0)
		, m_Height(0)
	{
	}

	bool Renderer::Initialize(uint32_t width, uint32_t height, std::shared_ptr<Graphics> pGraphics)
	{
		m_pGraphics = pGraphics;

		ResizeTargets(width, height);

		DV_LOG(Renderer, trace, "초기화 성공");

		return true;
	}

	void Renderer::Shutdown()
	{
		m_pGBuffer->Shutdown();

		DV_LOG(Renderer, trace, "셧다운 성공");
	}

	void Renderer::ResizeTargets(uint32_t width, uint32_t height)
	{
		if (m_Width == width && m_Height == height)
			return;

		m_Width = width;
		m_Height = height;

		m_pGBuffer->Resize(width, height);

		DV_LOG(Renerer, info, "Resize targets - {0:d} x {1:d}", m_Width, m_Height);
	}

	void Renderer::Update()
	{

	}

	void Renderer::Render()
	{
		// viewport
		D3D11_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, 1.0f };
		m_pGraphics->GetDeviceContext()->RSSetViewports(1, &viewport);

		// render targets
		ID3D11RenderTargetView* renderTargets[] = {
			m_pGBuffer->GetDiffuseTex()->GetRenderTargetView(),
			m_pGBuffer->GetNormalTex()->GetRenderTargetView(),
			m_pGBuffer->GetSpecularTex()->GetRenderTargetView()
		};
		ID3D11DepthStencilView* pDepthStencilView = m_pGBuffer->GetDepthTex()->GetDepthStencilView();

		float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		for (int i = 0; i != 3; i++)
		{
			m_pGraphics->GetDeviceContext()->ClearRenderTargetView(renderTargets[i], clearColor);
		}
		m_pGraphics->GetDeviceContext()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_pGraphics->GetDeviceContext()->OMSetRenderTargets(3, renderTargets, pDepthStencilView);
	}

	// 이건 Graphics에서 충분히 랩핑할 수 있을 것 같다.
	void Renderer::Present()
	{
		m_pGraphics->GetSwapChain()->Present(m_pGraphics->IsVSyncEnabled() ? 1 : 0, 0);
	}
}
