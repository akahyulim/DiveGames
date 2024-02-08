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

		DV_CORE_INFO("Engine �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Engine::Shutdown()
	{
		Scene::Shutdown();
		Input::Shutdown();
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();

		DV_CORE_INFO("Engine�� �����Ͽ����ϴ�.");
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
			// urho3d�� update, postupdata, renderupdate, postrenderupdate �� �� ���� �̺�Ʈ�θ� ó���ߴ�.
			// ����ý����� ���� ȣ���� ����.
			FIRE_EVENT(PreUpdateEvent());
			Timer::Update();
			Input::Update();
			Scene::Update();
			Renderer::Update();
			FIRE_EVENT(PostUpdateEvent());
		}

		// render
		{
			// urho3d�� begin/end ���̿�
			// renderer�� ui�� render�Լ��� ������� �����ߴ�.
			// ��, �̺�Ʈ�� ����.
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