#include "DivePch.h"
#include "Engine.h"
#include "DiveDefs.h"
#include "SystemManager.h"
#include "Timer.h"
#include "Rendering/Renderer.h"

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
		m_system_manager->RegisterSystem<Renderer>();

		if (!m_system_manager->Initialize())
			return false;

		return true;
	}
	
	void Engine::Update()
	{
		if (!m_system_manager)
			return;

		m_system_manager->Update();
	}
}
