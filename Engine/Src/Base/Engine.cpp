#include "divepch.h"
#include "Engine.h"
#include "Log.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Events/EventSystem.h"
#include "Events/EngineEvents.h"
#include "Renderer/RenderPath.h"

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
		// RenderPath�� Stop, Start�� �ִ�.

		// ���� ActiveRenderPath�� �ִٸ� Stop �� ���޹��� RenderPath�� Start ����� �Ѵ�.

		m_pActiveRenderPath = pRenderPath;
	}
}