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
		float deltaTime = 0.1f;

		Input::Update(deltaTime);
		Renderer::Update(deltaTime);
		Scene::Update(deltaTime);

		FIRE_EVENT(UpdateEvent(deltaTime));
	}

	void Engine::Render()
	{
		if (!Graphics::IsInitialized())
			return;

		Graphics::BeginFrame();

		Renderer::Render();

		Graphics::EndFrame();
	}

	bool Engine::IsExiting()
	{
		return s_bExiting;
	}
}