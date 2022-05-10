#include "divepch.h"
#include "Engine.h"
#include "Log.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Events/EventSystem.h"
#include "Events/EngineEvents.h"
#include "Renderer/RenderPath.h"
#include "Scene/Scene.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		auto hResult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		DV_ASSERT(SUCCEEDED(hResult));

		Log::Initialize();
		m_Time.Initialize();

		Renderer::Initialize(pData);
		Input::Initialize(pData->hWnd);
	}

	void Engine::Shutdown()
	{
		DV_DELETE(m_pActiveScene);
	
		Renderer::Shutdown();
	}

	void Engine::Update()
	{
		m_Time.Tick();

		auto delta = static_cast<float>(m_Time.GetDeltaTimeMS());

		Input::Update(delta);

		if (GetActiveRenderPath())
		{
			GetActiveRenderPath()->Update(delta);
		}
	}

	void Engine::Render()
	{
		if (GetActiveRenderPath())
		{
			GetActiveRenderPath()->Render();
		}
	}

	void Engine::SetWindowData(const WindowData& data)
	{
		m_WindowData = data;

		WindowDataEvent e(data);
		FIRE_EVENT(e);
	}

	void Engine::SetActiveRenderPath(RenderPath* pRenderPath)
	{
		// RenderPath에 Stop, Start가 있다.

		// 기존 ActiveRenderPath가 있다면 Stop 후 전달받은 RenderPath를 Start 해줘야 한다.

		m_pActiveRenderPath = pRenderPath;
	}

	Scene* Engine::CreateScene(const std::string& name)
	{
		// 일단은 지운다.
		if (m_pActiveScene)
		{
			DV_DELETE(m_pActiveScene);
		}

		m_pActiveScene = new Scene(name);
		DV_ASSERT(m_pActiveScene != nullptr);

		CreateSceneEvent e(m_pActiveScene);
		FIRE_EVENT(e);

		return m_pActiveScene;
	}
}