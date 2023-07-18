#include "DivePch.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "CoreEvents.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"
#include "IO/Log.h"

namespace Dive
{
	static bool s_bExiting = false;

	// temp
	static float s_DeltaTime = 0.0f;
	static float s_LastTime = 0.0f;

	bool Engine::Initialize()
	{
		Log::Initialize();

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

		DV_CORE_TRACE("Engine 초기화에 성공하였습니다.");

		return true;
	}

	void Engine::Shutdown()
	{
		Scene::Shutdown();
		Input::Shutdown();
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_TRACE("Engine 종료에 성공하였습니다.");
	}

	void Engine::RunFrame()
	{
		if (IsExiting())
			return;

		if (!Graphics::RunWindow())
		{
			s_bExiting = true;
			return;
		}

		Update();
		Render();
		
	}

	void Engine::Update()
	{
		// temp
		float currentTime = static_cast<float>(timeGetTime()) * 0.001f;
		s_DeltaTime = currentTime - s_LastTime;
		s_LastTime = currentTime;

		// 순서가 중요하다. 특히 Scene의 업데이트 후 Renderer가 업데이트 되어야 한다.
		Input::Update(s_DeltaTime);
		Scene::Update(s_DeltaTime);
		Renderer::Update(s_DeltaTime);

		FIRE_EVENT(UpdateEvent(s_DeltaTime));
	}

	void Engine::Render()
	{
		if (!Graphics::IsInitialized())
			return;

		Graphics::BeginFrame();

		Renderer::Render();

		Graphics::EndFrame();
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