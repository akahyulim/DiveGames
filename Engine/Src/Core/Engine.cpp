#include "DivePch.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "CoreEvents.h"
#include "Timer.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"

namespace Dive
{
	static bool s_bExiting = false;

	bool Engine::Initialize()
	{
		if (!Graphics::Initialize())
			return false;

		if (!Renderer::Initialize())
			return false;

		if (!ResourceCache::Initialize())
			return false;

		if (!Input::Initialize(Graphics::GetWindowInstance(), Graphics::GetWindowHandle()))
			return false;

		if (!Scene::Initialize())
			return false;

		DV_CORE_INFO("Engine 초기화에 성공하였습니다.");

		return true;
	}

	void Engine::Shutdown()
	{
		Scene::Shutdown();
		Input::Shutdown();
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_INFO("Engine을 종료하였습니다.");
	}
	
	void Engine::Tick()
	{
		// pre update
		FIRE_EVENT(PreUpdateEvent());

		// update
		if (!Graphics::RunWindow())
		{
			s_bExiting = true;
			return;
		}
		Timer::Update();
		Input::Update();
		Scene::Update();
		Renderer::Update();
		FIRE_EVENT(UpdateEvent());

		// post update
		Graphics::Present();
		FIRE_EVENT(PostUpdateEvent());
	}

	void Engine::Exit()
	{
		s_bExiting = true;
	}

	bool Engine::IsExiting()
	{
		return s_bExiting;
	}
}