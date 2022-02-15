#include "divepch.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Base/Engine.h"

namespace Dive
{
	void Renderer::Initialize(const WindowData* pData)
	{
		m_GraphicsDevice.Initialize(pData);
	}

	void Renderer::Shutdow()
	{
		m_GraphicsDevice.Shutdown();
	}

	void Renderer::Tick()
	{
		// update
		{}

		// render
		{
			BeginScene();

			EndScene();
		}
	}
	
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pMainRenderTargetView = m_GraphicsDevice.GetMainRenderTargetView();
		if (!pImmediateContext || !pMainRenderTargetView)
			return;

		// active camera�κ��� �����;� �Ѵ�.
		float clearColors[4] = {1.0f, 1.0f, 1.0f, 1.0f};

		pImmediateContext->ClearRenderTargetView(pMainRenderTargetView, clearColors);
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
		// �ٸ� RenderTarget�鵵 ���� ����̴�.

		m_GraphicsDevice.ResizeBackBuffer(width, height);
	}

	ID3D11RenderTargetView* Renderer::GetMainRenderTargetView()
	{
		return m_GraphicsDevice.GetMainRenderTargetView();
	}
}