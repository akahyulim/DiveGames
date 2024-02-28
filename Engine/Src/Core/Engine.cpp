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
	static bool s_bInitialized = false;
	static bool s_bExit = false;

	bool Engine::Initialize(uint32_t width, uint32_t height, bool fullScreen)
	{
		Log::Initialize();

		if (!Graphics::SetScreenMode(width, height, fullScreen, false))
			return false;

		if (!Renderer::Initialize())
			return false;

		if (!Input::Initialize())
			return false;

		s_bInitialized = true;
		DV_CORE_TRACE("엔진 초기화 성공");
		return true;
	}

	void Engine::Shutdown()
	{
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_TRACE("엔진 셧다운");
	}
	
	void Engine::RunFrame()
	{
		if (!Graphics::RunWindow())
		{
			s_bExit = true;
			return;
		}

		OnUpdate();

		OnRender();
	}
	
	void Engine::OnUpdate()
	{
		FIRE_EVENT(PreUpdateEvent());

		Timer::Update();
		Input::Update();
		Renderer::Update();

		FIRE_EVENT(PostUpdateEvent());
	}
	
	void Engine::OnRender()
	{
		Graphics::BeginFrame();
		
		FIRE_EVENT(PreRenderEvent());

		Renderer::Render();

		FIRE_EVENT(PostRenderEvent());

		Graphics::EndFrame();
	}
	
	bool Engine::IsInitialized()
	{
		return s_bInitialized;
	}
	
	bool Engine::IsExit()
	{
		return s_bExit;
	}
}