#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		Log::Initialize();
		m_Time.Initialize();

		Renderer::Initialize(pData);
	}

	void Engine::Shutdown()
	{
		Renderer::Shutdown();
	}

	void Engine::Run()
	{
		m_Time.Tick();
	}

	void Engine::SetWindowData(const WindowData& data)
	{
		m_WindowData = data;

		// 이후 이벤트로 전달한다.
		// 그런데 Hazel과 Spartan의 Event System이 다르다.
		// 어찌됐든 메시지별로 이벤트를 나눠야 한다.

		// 임시 처리
		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		// 이렇게 메시지까지 확인하는 것 보단
		// 단순 크기 비교가 더 맞아 보였는데
		// 직접 구현해보니 깜빡인다...
		if (data.msg == WM_SIZE && data.wParam != SIZE_MINIMIZED)
		{
			// 그리고 함수에 크기를 전달하는 것이 맞다.
			graphicsDevice.CleanupMainRenderTargetView();
			graphicsDevice.GetSwapChain()->ResizeBuffers(0, data.Width, data.Height, DXGI_FORMAT_UNKNOWN, 0);
			graphicsDevice.CreateMainRenderTargetView();
		}
	}
}