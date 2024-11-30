#include "divepch.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "StringUtils.h"
#include "Window.h"
#include "CoreEvents.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceManager.h"
#include "Input/Input.h"
#include "World/SceneManager.h"	// ���� ���
#include "World/Scene.h"

namespace Dive
{
	Engine* Engine::s_pInstance = nullptr;

	Engine::Engine()
		: m_bInitialized(false)
		, m_bExit(false)
		, m_TimeMS(0.0)
		, m_DeltaTimeMS(0.0)
		, m_LastTickTime(std::chrono::steady_clock::now())
		, m_FrameCount(0)
		, m_LastTime(0)
		, m_Fps(0.0)
	{
	}

	bool Engine::Initialize(uint32_t width, uint32_t height, bool fullScreen)
	{
		//Log::GetInstance()->Initialize(spdlog::level::trace, spdlog::level::trace);

		if (!Graphics::GetInstance()->Initialize(width, height, fullScreen, false))
		{
			DV_LOG(Engine, critical, "�׷��Ƚ� �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}

		if (!Renderer::GetInstance()->Initialize())
		{
			DV_LOG(Engine, critical, "������ �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}

		if (!Input::GetInstance()->Initialize())
		{
			DV_LOG(Engine, critical, "��ǲ �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}

		m_bInitialized = true;

		DV_LOG(Engine, trace,  "���� �ʱ�ȭ ����.");

		return true;
	}

	void Engine::Shutdown()
	{
		SceneManager::GetInstance()->Shutdown();	// ���� ������� �ʴ� ��ü�� ���� ��� ������ ������ �ʿ�� ����δ�.
		ResourceManager::GetInstance()->Shutdown();

		Input::GetInstance()->Shutdown();
		Renderer::GetInstance()->Shutdown();
		Graphics::GetInstance()->Shutdown();

		DV_LOG(Engine, trace,  "���� �˴ٿ �����Ͽ����ϴ�.");
	}
	
	void Engine::RunFrame()
	{
		if (!Graphics::GetInstance()->RunWindow())
		{
			m_bExit = true;
			return;
		}

		m_DeltaTimeMS = static_cast<double>(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_LastTickTime).count());
		m_TimeMS += m_DeltaTimeMS;
		m_LastTickTime = std::chrono::steady_clock::now();

		m_FrameCount++;
		if (m_TimeMS - m_LastTime >= 1000.0)
		{
			m_Fps = m_FrameCount;
			m_FrameCount = 0;
			m_LastTime = m_TimeMS;
		}

		OnUpdate();
		OnRender();
	}
	
	void Engine::OnUpdate()
	{
		FIRE_EVENT(PreUpdateEvent());

		Renderer::GetInstance()->Update();
		Input::GetInstance()->Update();

		FIRE_EVENT(PostUpdateEvent());
	}
	// Update�� Render�� Ȯ�强�� ����� �ۿ��� ���� ȣ���ϵ��� ����� ���� ���� �� ����.
	void Engine::OnRender()
	{
		Graphics::GetInstance()->BeginFrame();
		
		FIRE_EVENT(PreRenderEvent());

		Renderer::GetInstance()->Render();

		FIRE_EVENT(PostRenderEvent());

		Graphics::GetInstance()->EndFrame();
	}
	
	bool Engine::IsInitialized()
	{
		return m_bInitialized;
	}
	
	bool Engine::IsExit()
	{
		return m_bExit;
	}

	double Engine::GetFps()
	{
		return m_Fps;
	}

	double Engine::GetTimeMS()
	{
		return m_TimeMS;
	}

	double Engine::GetTimeSec()
	{
		return m_TimeMS / 1000.0;
	}

	double Engine::GetDeltaTimeMS()
	{
		return m_DeltaTimeMS;
	}

	double Engine::GetDeltaTimeSec()
	{
		return m_DeltaTimeMS / 1000.0;
	}

	Engine* GetEngine()
	{
		if (Engine::GetInstance()->IsInitialized())
			return Engine::GetInstance();

		return nullptr;
	}
}