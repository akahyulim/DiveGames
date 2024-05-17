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

		// 구조체가 가장 어울린다.
		// 파일을 로드하거나 OnSet()에서 설정 가능해야 하므로
		// 멤버 변수여야 한다.
		if (!m_pEngine->Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
		{
			DV_CRITICAL("엔진 초기화에 실패하여 프로그램을 종료합니다.");
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