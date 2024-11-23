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

		// ����ü �Ӹ� �ƴ϶� �پ��� ������ �����ϵ��� �����ε��ؾ� �Ѵ�.
		if (!m_pEngine->Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			DV_LOG(Application, critical, "�ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
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