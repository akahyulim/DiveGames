#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "EventDispatcher.h"
#include "graphics/Graphics.h"
#include "rendering/Renderer.h"
#include "input/Input.h"

namespace Dive
{
	Engine* GEngine = Engine::GetInstance();

	Engine* Engine::s_pInstance = nullptr;
	std::once_flag Engine::s_OnceFlag;

	Engine::Engine()
		: m_pWindow(std::make_unique<Window>())
		, m_pGraphics(std::make_shared<Graphics>())
		, m_pRenderer(std::make_unique<Renderer>())
		, m_pInput(std::make_unique<Input>())
	{
	}

	bool Engine::Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle)
	{
		DV_LOG(Engine, trace, "초기화 시작");

		if (!m_pWindow->Initialize(hInstance, width, height, pTitle))
			return false;

		if (!m_pGraphics->Initialize(width, height, m_pWindow->GetHandle()))
			return false;

		if (!m_pRenderer->Initialize(width, height, m_pGraphics))
			return false;

		if (!m_pInput->Initialize(hInstance, m_pWindow->GetHandle()))
			return false;

		DV_SUBSCRIBE_EVENT(eEventType::Exit, DV_EVENT_HANDLER(OnExit));

		DV_LOG(Engine, trace, "초기화 완료");

		return true;
	}

	void Engine::Shutdown()
	{
		DV_LOG(Engine, trace, "셧다운 시작");
	
		m_pInput->Shutdown();
		m_pRenderer->Shutdown();
		m_pGraphics->Shutdown();
		m_pWindow->Shutdown();

		DV_LOG(Engine, trace, "셧다운 종료");
	}

	void Engine::Tick()
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
				m_pInput->Tick();

				m_pRenderer->Tick();
			}
		}
	}

	void Engine::OnExit()
	{
		::PostQuitMessage(0);
	}

	HWND Engine::GetWindowHandle()
	{
		return m_pWindow->GetHandle();
	}

	IDXGISwapChain* Engine::GetSwapChain()
	{
		return m_pGraphics->GetSwapChain();
	}

	ID3D11Device* Engine::GetDevice()
	{
		return m_pGraphics->GetDevice();
	}
	
	ID3D11DeviceContext* Engine::GetDeviceContext()
	{
		return m_pGraphics->GetDeviceContext();
	}
}