#include "divepch.h"
#include "Engine.h"
#include "Log.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Events/EventSystem.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		Log::Initialize();
		m_Time.Initialize();

		Renderer::Initialize(pData);
		Input::Initialize(pData->hWnd);
	}

	void Engine::Shutdown()
	{
		Renderer::Shutdown();
	}

	void Engine::Run()
	{
		m_Time.Tick();
		Input::Update(0.0f);

		if (Input::KeyDown(VK_RBUTTON))
		{
			auto pos = Input::GetMousePosition();
			DV_CORE_INFO("MousePos: {0:f}, {1:f}", pos.x, pos.y);
		}

		if (Input::KeyDown(VK_UP))
		{
			Input::SetMousePosition(0.0f, 0.0f);
			auto pos = Input::GetMousePosition();
			DV_CORE_INFO("MousePos: {0:f}, {1:f}", pos.x, pos.y);
		}
	}

	void Engine::SetWindowData(const WindowData& data)
	{
		m_WindowData = data;

		WindowDataEvent e(data);
		FIRE_EVENT(e);
	}
}