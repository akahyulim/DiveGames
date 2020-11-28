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
		Log::Initialize();
	}

	Engine::Engine(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed)
	{
		// Set WindowData
		{
			m_windowData.hInstance	= hInstance;
			m_windowData.hWnd		= hWnd;
			m_windowData.width		= width;
			m_windowData.height		= height;
			m_windowData.windowed	= windowed;
		}

		// Initialize Priority Object
		{
			// 얘네는 이름을 바꾸고 싶다.
			Log::Initialize();
			//Settings::GetInstance().Initialize();
		}

		// Create & Initialize Systems
		{
			m_systemManager = std::make_shared<SystemManager>(this);
			m_systemManager->RegisterSystem<Timer>();
			m_systemManager->RegisterSystem<Renderer>();

			if (!m_systemManager->Initialize())
			{
				return;
			}
		}

		m_bInitialized = true;
	}

	Engine::~Engine()
	{
		CORE_TRACE("Call Engine's Destructor ========================");
		CORE_TRACE("SystemManager's Use Count : {:d}", m_systemManager.use_count());
	}
	
	void Engine::Update()
	{
		if (!m_systemManager)
			return;

		m_systemManager->Update();
	}
}
