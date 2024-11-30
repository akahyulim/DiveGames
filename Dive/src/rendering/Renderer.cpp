#include "stdafx.h"
#include "Renderer.h"
//#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	Renderer::Renderer()
		: m_pGraphics(std::make_unique<Graphics>())
	{
	}

	bool Renderer::Initialize(uint32_t width, uint32_t height, HWND hWnd)
	{
		if (!m_pGraphics->Initialize(width, height, hWnd))
			return false;

		DV_LOG(Renderer, trace, "초기화 성공");

		return true;
	}

	void Renderer::Shutdown()
	{
		m_pGraphics->Shutdown();

		DV_LOG(Renderer, trace, "셧다운 성공");
	}

	void Renderer::Update()
	{

	}

	void Renderer::Render()
	{
		// 테스트용
		float clearColor[4] = { 0.75f, 0.75f, 1.0f, 1.0f };
		GetDeviceContext()->ClearRenderTargetView(m_pGraphics->GetBackbufferView(), clearColor);
		GetDeviceContext()->ClearDepthStencilView(m_pGraphics->GetBackbufferDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* views[] = { m_pGraphics->GetBackbufferView() };
		GetDeviceContext()->OMSetRenderTargets(1, views, m_pGraphics->GetBackbufferDepthView());
	}

	// 이건 Graphics에서 충분히 랩핑할 수 있을 것 같다.
	void Renderer::Present()
	{
		m_pGraphics->GetSwapChain()->Present(m_pGraphics->IsVSyncEnabled() ? 1 : 0, 0);
	}

	ID3D11Device* Renderer::GetDevice()
	{
		return m_pGraphics->GetDevice();
	}
	
	ID3D11DeviceContext* Renderer::GetDeviceContext()
	{
		return m_pGraphics->GetDeviceContext();
	}
}
