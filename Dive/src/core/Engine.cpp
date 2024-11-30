#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "EventDispatcher.h"
#include "rendering/Renderer.h"

namespace Dive
{
	Engine::Engine()
		: m_pWindow(std::make_unique<Window>())
		, m_pRenderer(std::make_unique<Renderer>())
	{
	}

	bool Engine::Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle)
	{
		DV_LOG(Engine, trace, "초기화 시작");

		if (!m_pWindow->Initialize(hInstance, width, height, pTitle))
			return false;

		if (!m_pRenderer->Initialize(width, height, m_pWindow->GetHandle()))
			return false;

		DV_LOG(Engine, trace, "초기화 완료");

		return true;
	}

	void Engine::Run()
	{
		MSG msg{};
		while (msg.message != WM_QUIT)
		{
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
			else
			{
				DV_FIRE_EVENT(eEventType::PreRender);
				m_pRenderer->Render();
				DV_FIRE_EVENT(eEventType::PostRender);
				m_pRenderer->Present();
			}
		}
	}

	void Engine::Shutdown()
	{
		DV_LOG(Engine, trace, "셧다운 시작");
	
		m_pRenderer->Shutdown();
		m_pWindow->Shutdown();

		DV_LOG(Engine, trace, "셧다운 종료");
	}

	HWND Engine::GetWindowHandle()
	{
		return m_pWindow->GetHandle();
	}

	ID3D11Device* Engine::GetDevice()
	{
		return m_pRenderer->GetDevice();
	}
	
	ID3D11DeviceContext* Engine::GetDeviceContext()
	{
		return m_pRenderer->GetDeviceContext();
	}
}