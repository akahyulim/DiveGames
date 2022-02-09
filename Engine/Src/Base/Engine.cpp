#include "divepch.h"
#include "Engine.h"
#include "Base.h"
#include "Time.h"

namespace Dive
{
	Engine* Engine::s_pInstance = nullptr;

	Engine::Engine()
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;
	}

	Engine::~Engine()
	{
		// sub system 종료
	}

	void Engine::Initialize()
	{
		// sub system 초기화
		Log::Initialize();
	}

	void Engine::Update()
	{
	}

	void Engine::Render()
	{
	}

	void Engine::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(DV_BIND_EVENT_FN(Engine::OnWinodwResize));
	}

	bool Engine::OnWinodwResize(WindowResizeEvent& event)
	{
		CORE_TRACE("call wnd resize event: {0:d} x {1:d}", event.GetWidth(), event.GetHeight());

		return true;
	}
}