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
		// ��״� �̸��� �ٲٰ� �ʹ�.
		Log::Initialize();
		Settings::GetInstance().Initialize();

		// ��ü ������ ���������� ���� �� ����.====================
		m_SystemManager = std::make_shared<SystemManager>(this);
		m_SystemManager->RegisterSystem<Timer>();
		m_SystemManager->RegisterSystem<Renderer>();
		// ========================================================
	}

	Engine::~Engine()
	{
	}
	
	bool Engine::Initialize(const WindowData& windowData)
	{
		m_WindowData = windowData;

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
