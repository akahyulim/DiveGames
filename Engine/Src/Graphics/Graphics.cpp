#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "Core/Context.h"
#include "Core/Log.h"


namespace Dive
{
	Graphics::Graphics(Context* context)
		: Object(context)

	{
		m_window = std::make_shared<Window>();
	}

	Graphics::~Graphics()
	{
		m_window->Destroy();
		m_window.reset();
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		// 댕글링 포인터는 어떻게....?
		if (!m_window->Run())
			return false;

		// render target clear
		float clear_color[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
		m_immediateContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
		m_immediateContext->ClearRenderTargetView(m_renderTargetView, (float*)&clear_color);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!m_swapChain)
		{
			CORE_ERROR("");
			return;
		}

		UINT syncInterval = 0;
		//if (m_bVSync)
			syncInterval = 1;
		if (FAILED(m_swapChain->Present(syncInterval, 0)))
		{
			CORE_ERROR("");
			return;
		}
	}

	bool Graphics::IsInitialized()
	{
		return ( m_window != nullptr && m_device != nullptr);
	}

	// Engine::Initialize -> Graphics::SetMode -> SetDefaultWindowModes -> SetWindowMode -> SetScreenMode(Window 생성, RHI 생성)
	bool Graphics::SetScreenMode()
	{
		// 윈도우 생성
		if (!m_window->Create(1024, 768))
			return false;
		m_window->Show();

		if (!createRHI())
			return false;

		return true;
	}
}
