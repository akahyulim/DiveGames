#include "DivePch.h"
#include "Engine.h"
#include "DiveDefs.h"
#include "SystemManager.h"
#include "Timer.h"

namespace Dive
{
	Engine::Engine()
	{
	}

	Engine::~Engine()
	{
	}
	
	bool Engine::Initialize()
	{
		m_system_manager = std::make_shared<SystemManager>(this);

		m_system_manager->RegisterSystem<Timer>();

		if (!m_system_manager->Initialize())
			return false;

		return true;
	}
	
	void Engine::Update()
	{
		if (!m_system_manager)
			return;

		auto timer = m_system_manager->GetSystem<Timer>();

		m_system_manager->Update(timer->GetDeltaTimeMS());
	}
}
