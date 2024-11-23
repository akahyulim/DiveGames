#include "divepch.h"
#include "Application.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Engine.h"

namespace Dive
{
	Application::Application()
	//	: m_pEngine(Engine::GetInstance())
	//	, m_Title(L"Dive")
		: m_Title(L"Dive")
		, m_WindowWidth(800)
		, m_WindowHeight(600)
		, m_bFullscreen(false)
	{
	}

	int Application::Run()
	{
		OnSetup();

		OnStart();

		while (GEngine->IsRun())
			OnTick();

		OnStop();
		
		return 0;
	}

	/*
	int Application::Run()
	{
		OnSetup();

		// 구조체 뿐만 아니라 다양한 조합을 지원하도록 오버로딩해야 한다.
		if (!m_pEngine->Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			DV_LOG(Application, critical, "초기화에 실패하여 프로그램을 종료합니다.");
			return -1;
		}

		OnStart();

		while (!m_pEngine->IsExit())
			OnTick();

		OnStop();

		m_pEngine->Shutdown();

		return 0;
	}
	*/
}