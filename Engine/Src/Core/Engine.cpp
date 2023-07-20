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

		DV_CORE_TRACE("Engine �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Engine::Shutdown()
	{
		Scene::Shutdown();
		Input::Shutdown();
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_TRACE("Engine ���ῡ �����Ͽ����ϴ�.");
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
		// ������ �߿��ϴ�. Ư�� Scene�� ������Ʈ �� Renderer�� ������Ʈ �Ǿ�� �Ѵ�.
		Input::Update();
		Scene::Update();
		Renderer::Update();

		Timer::Update();

		FIRE_EVENT(UpdateEvent());
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