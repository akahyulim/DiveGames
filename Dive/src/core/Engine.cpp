#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "Timer.h"
#include "EventDispatcher.h"
#include "graphics/Graphics.h"
#include "graphics/ShaderManager.h"
#include "rendering/Renderer.h"
#include "world/WorldManager.h"

namespace Dive
{
	bool Engine::Initialize()
	{
		DV_LOG(Engine, info, "초기화 시작");

		Timer::Initialize();

		if (!Window::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Window 초기화 실패");
			m_bExit = true;
			return false;
		}

		if (!Graphics::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Graphics 초기화 실패");
			m_bExit = true;
			return false;
		}

		if (!ShaderManager::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] ShaderManager 초기화 실패");
			m_bExit = true;
			return false;
		}

		if (!Renderer::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Renderer 초기화 실패");
			m_bExit = true;
			return false;
		}

		if (!Input::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Input 초기화 실패");
			m_bExit = true;
			return false;
		}

		if (!WorldManager::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] WorldManager 초기화 실패");
			m_bExit = true;
			return false;
		}

		DV_LOG(Engine, info, "초기화 완료");

		return true;
	}

	void Engine::Shutdown()
	{
		DV_LOG(Engine, info, "셧다운 시작");

		WorldManager::Clear();
		ShaderManager::Clear();
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
		Window::Shutdown();

		DV_LOG(Engine, info, "셧다운 완료");

		LogManager::Shutdown();
	}

	void Engine::RunFrame()
	{
		// 서브 시스템의 초기화를 확인
		// urho3d는 graphics의 초기화여부로 m_bExit 판별
		if (m_bExit)
			return;

		if (Window::Run())
		{
			DV_FIRE_EVENT(eEventType::BeginFrame);

			// Update
			{
				Timer::Tick();
			
				DV_FIRE_EVENT_DATA(eEventType::PreUpdate, Timer::GetDeltaTimeSec());
				DV_FIRE_EVENT_DATA(eEventType::Update, Timer::GetDeltaTimeSec());
				DV_FIRE_EVENT_DATA(eEventType::PostUpdate, Timer::GetDeltaTimeSec());
			}

			// Render
			{
				DV_FIRE_EVENT_DATA(eEventType::PreRender, Timer::GetDeltaTimeSec());
				DV_FIRE_EVENT_DATA(eEventType::Render, Timer::GetDeltaTimeSec());
				DV_FIRE_EVENT_DATA(eEventType::PostRender, Timer::GetDeltaTimeSec());
			}

			Graphics::Present();

			DV_FIRE_EVENT(eEventType::EndFrame);
		}
		else
			DoExit();
	}

	void Engine::DoExit()
	{
		m_bExit = true;
	}
}
