#include "DivePch.h"
#include "Engine.h"
#include "DiveDefs.h"
#include "SystemManager.h"
#include "Timer.h"
#include "Settings.h"
#include "Rendering/Renderer.h"

namespace Dive
{
	Engine::Engine(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed)
	{
		m_WindowData.hInstance	= hInstance;
		m_WindowData.hWnd		= hWnd;
		m_WindowData.width		= width;
		m_WindowData.height		= height;
		m_WindowData.windowed	= windowed;

		// ��״� �̸��� �ٲٰ� �ʹ�.
		Log::Initialize();
		//Settings::GetInstance().Initialize();

		m_SystemManager = std::make_shared<SystemManager>(this);
		m_SystemManager->RegisterSystem<Timer>();
		m_SystemManager->RegisterSystem<Renderer>();

		if (!m_SystemManager->Initialize())
		{
			return;
		}

		m_bInitialize = true;
	}

	Engine::~Engine()
	{
	}
	
	void Engine::Update()
	{
		if (!m_SystemManager)
			return;

		m_SystemManager->Update();
	}
}
