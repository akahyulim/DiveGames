#include "divepch.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Timer.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"
#include "CoreEvents.h"

namespace Dive
{
	Engine::Engine()
		: m_bInitialized(false)
		, m_bExit(false)
	{
	}

	Engine::~Engine()
	{
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_TRACE("엔진 종료");
	}
	
	bool Engine::Initialize(uint32_t width, uint32_t height, bool fullScreen)
	{
		Log::Initialize();

		if (!Graphics::SetScreenMode(width, height, fullScreen, false))
			return false;

		if (!Renderer::Initialize())
			return false;

		if (!Input::Initialize())
			return false;

		m_bInitialized = true;
		DV_CORE_TRACE("엔진 초기화 성공");
		return true;
	}

	void Engine::RunFrame()
	{
		if (!Graphics::RunWindow())
		{
			m_bExit = true;
			return;
		}

		OnUpdate();

		OnRender();
	}

	void Engine::OnUpdate()
	{
		Timer::Update();
		Input::Update();
		Renderer::Update();

		FIRE_EVENT(PostUpdateEvent());
	}
	
	void Engine::OnRender()
	{
		Graphics::BeginFrame();
		Renderer::Render();
		Graphics::EndFrame();
	}
}