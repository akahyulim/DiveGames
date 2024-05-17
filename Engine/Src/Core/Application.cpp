#include "divepch.h"
#include "Application.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Engine.h"

namespace Dive
{
	Application::Application()
		: m_pEngine(Engine::GetInstance())
		, m_Title(L"Dive")
		, m_WindowWidth(800)
		, m_WindowHeight(600)
		, m_bFullscreen(false)
	{
	}

	int Application::Run()
	{
		OnSetup();

		// ����ü�� ���� ��︰��.
		// ������ �ε��ϰų� OnSet()���� ���� �����ؾ� �ϹǷ�
		// ��� �������� �Ѵ�.
		if (!m_pEngine->Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			DV_CRITICAL("���� �ʱ�ȭ�� �����Ͽ� ���α׷��� �����մϴ�.");
			return -1;
		}

		OnStart();

		while (!m_pEngine->IsExit())
			m_pEngine->RunFrame();

		OnStop();

		m_pEngine->Shutdown();

		return 0;
	}
}