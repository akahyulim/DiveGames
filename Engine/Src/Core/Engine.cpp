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

		if (!Input::Initialize())
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
		if (!Graphics::RunWindow())
		{
			s_bExiting = true;
			return;
		}

		// update
		{	
			// urho3d는 update, postupdata, renderupdate, postrenderupdate 총 네 개의 이벤트로만 처리했다.
			// 서브시스템의 직접 호출은 없다.
			FIRE_EVENT(PreUpdateEvent());
			Timer::Update();
			Input::Update();
			Scene::Update();
			Renderer::Update();
			FIRE_EVENT(PostUpdateEvent());
		}

		// render
		{
			// urho3d는 begin/end 사이에
			// renderer와 ui의 render함수를 순서대로 실행했다.
			// 즉, 이벤트는 없다.
			FIRE_EVENT(PreRenderEvent());
			Renderer::Render();
			FIRE_EVENT(PostRenderEvent());	
			Graphics::Present();
		}
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