#include "DivePch.h"
#include "Engine.h"
#include "DiveDefs.h"
#include "SystemManager.h"
#include "Timer.h"
#include "Settings.h"
#include "Rendering/Renderer.h"

namespace Dive
{
	Engine::Engine()
	{
		// test
		Log::Initialize();
		Settings::GetInstance().Initialize();
	}

	Engine::~Engine()
	{
	}
	
	bool Engine::Initialize(const WindowData& windowData)
	{
		m_WindowData = windowData;

		// ��ü ������ ���������� ���� �� ����.====================
		m_SystemManager = std::make_shared<SystemManager>(this);
		m_SystemManager->RegisterSystem<Timer>();
		m_SystemManager->RegisterSystem<Renderer>();
		// ========================================================

		if (!m_SystemManager->Initialize())
			return false;

		return true;
	}
	
	void Engine::Update()
	{
		if (!m_SystemManager)
			return;

		m_SystemManager->Update();
	}
}
