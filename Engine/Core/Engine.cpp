#include "DivePch.h"
#include "Engine.h"
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
		m_pSystemManager = new SystemManager(this);

		m_pSystemManager->RegisterSystem(static_cast<ISystem*>(new Timer(m_pSystemManager)));

		if (!m_pSystemManager->Initialize())
			return false;

		return true;
	}
	
	void Engine::Update()
	{
		if (!m_pSystemManager)
			return;

		auto pTimer = m_pSystemManager->GetSystem<Timer>();

		m_pSystemManager->Update(pTimer->GetDeltaTimeMS());
	}
}
