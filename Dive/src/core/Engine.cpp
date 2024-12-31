#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "EventDispatcher.h"
#include "graphics/Graphics.h"
#include "rendering/Renderer.h"
#include "input/Input.h"
#include "world/World.h"

namespace Dive
{
	Engine* GEngine = Engine::GetInstance();

	Engine* Engine::s_pInstance = nullptr;
	std::once_flag Engine::s_OnceFlag;

	Engine::Engine()
		: m_pWindow(std::make_shared<Window>())
		, m_pGraphics(std::make_shared<Graphics>())
		, m_pRenderer(std::make_shared<Renderer>())
		, m_pInput(std::make_shared<Input>())
		, m_ElapsedTimeMS(0.0)
		, m_DeltaTimeMS(0.0f)
		, m_Fps(0)
	{
	}

	bool Engine::Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle)
	{
		DV_LOG(Engine, trace, "초기화 시작");

		// 일반적으로 객체의 생성과 초기화는 생성자에서 수행한다.
		// Initialize, Create 등은 싱글톤, 정적 클래스들에서 사용한다.
		if (!m_pWindow->Initialize(hInstance, width, height, pTitle))
			return false;

		if (!m_pGraphics->Initialize(width, height, GetWindowHandle()))
			return false;

		if (!m_pRenderer->Initialize(width, height, m_pGraphics))
			return false;

		if (!m_pInput->Initialize(hInstance, GetWindowHandle()))
			return false;

		m_LastTickTime = std::chrono::steady_clock::now();

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

		DV_LOG(Engine, trace, "셧다운 종료(elapsed time: {:.2f} sec)", GetElapsedTimeSec());
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
				auto currentTickTime = std::chrono::steady_clock::now();
				m_DeltaTimeMS = std::chrono::duration<float, std::milli>(currentTickTime - m_LastTickTime).count();
				m_ElapsedTimeMS += m_DeltaTimeMS;
				m_LastTickTime = currentTickTime;

				static double lastTimeMS = 0;
				static uint16_t frameCount = 0;
				frameCount++;
				if (m_ElapsedTimeMS - lastTimeMS >= 1000.0)
				{
					m_Fps = frameCount;
					frameCount = 0;
					lastTimeMS = m_ElapsedTimeMS;
				}

				m_pInput->Tick();
				//m_pActiveWorld->Tick();
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

	std::shared_ptr<World> Engine::CreateWorld(const std::string& name)
	{
		if (m_pActiveWorld)
		{
			m_pActiveWorld.reset();
		}

		m_pActiveWorld = std::make_shared<World>(name);

		return m_pActiveWorld;
	}

	std::shared_ptr<World> Engine::OpenWorld(const std::filesystem::path& path)
	{
		if (m_pActiveWorld)
		{
			if (m_pActiveWorld->GetPath() == path)
				return m_pActiveWorld;

			m_pActiveWorld.reset();
		}

		m_pActiveWorld = std::make_shared<World>();
		m_pActiveWorld->LoadFromFile(path);

		return m_pActiveWorld;
	}

	void Engine::CloseWorld()
	{
		if (!m_pActiveWorld)
			return;

		m_pActiveWorld->Clear();
		m_pActiveWorld.reset();
	}

	double Engine::GetElapsedTimeMS() const
	{
		return m_ElapsedTimeMS;
	}

	double Engine::GetElapsedTimeSec() const
	{
		return m_ElapsedTimeMS / 1000.0;
	}

	float Engine::GetDeltaTimeMS() const
	{
		return m_DeltaTimeMS;
	}

	float Engine::GetDeltaTimeSec() const
	{
		return m_DeltaTimeMS / 1000.0f;
	}

	uint16_t Engine::GetFps() const
	{
		return m_Fps;
	}
}