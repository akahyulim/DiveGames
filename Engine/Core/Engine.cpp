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
		if (m_system_manager)
			SAFE_DELETE(m_system_manager);
	}
	
	bool Engine::Initialize()
	{
		m_system_manager = new SystemManager(this);

		m_system_manager->RegisterSystem(static_cast<ISystem*>(new Timer(m_system_manager)));

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
