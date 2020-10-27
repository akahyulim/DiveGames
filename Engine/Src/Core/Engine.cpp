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
		m_pSystemManager = std::make_shared<SystemManager>(this);

		m_pSystemManager->RegisterSystem<Timer>();

		if (!m_pSystemManager->Initialize())
			return false;

		return true;
	}
	
	void Engine::Update()
	{
		if (!m_pSystemManager)
			return;

		m_pSystemManager->Update();
	}
}
