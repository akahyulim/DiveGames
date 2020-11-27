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
			m_WindowData.hInstance	= hInstance;
			m_WindowData.hWnd		= hWnd;
			m_WindowData.width		= width;
			m_WindowData.height		= height;
			m_WindowData.windowed	= windowed;
		}

		// Initialize Priority Object
		{
			// 얘네는 이름을 바꾸고 싶다.
			Log::Initialize();
			//Settings::GetInstance().Initialize();
		}

		// Create & Initialize Systems
		{
			m_SystemManager = std::make_shared<SystemManager>(this);
			m_SystemManager->RegisterSystem<Timer>();
			m_SystemManager->RegisterSystem<Renderer>();

			if (!m_SystemManager->Initialize())
			{
				return;
			}
		}

		m_bInitialized = true;
	}

	Engine::~Engine()
	{
		CORE_TRACE("Call Engine's Destructor ========================");
		CORE_TRACE("SystemManager's Use Count : {:d}", m_SystemManager.use_count());
	}
	
	void Engine::Update()
	{
		if (!m_SystemManager)
			return;

		m_SystemManager->Update();
	}
}
