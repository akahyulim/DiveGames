#include "divepch.h"
#include "Engine.h"
#include "Base.h"
#include "Time.h"
#include "Events/Event.h"

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
		// sub system ����
	}

	void Engine::Initialize()
	{
		// sub system �ʱ�ȭ
		Log::Initialize();
	}

	void Engine::Destroy()
	{
	}

	void Engine::Tick()
	{
	}

	void Engine::OnEvent(Event& event)
	{
	}
}