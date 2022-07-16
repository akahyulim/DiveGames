#include "divepch.h"
#include "Application.h"
#include "DvEngine.h"

namespace Dive
{
	Application::Application(DvContext* pContext)
		: DvObject(pContext),
		m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters: 이건 아직 뭔지 모르겠다.

		m_pEngine = new DvEngine(pContext);

	}

	int Application::Run()
	{
		Setup();
		if (m_ExitCode)
			return m_ExitCode;

		// 파라미터를 매개인자로 전달해야 한다.
		if(!m_pEngine->Initialize())
		{
			ErrorExit();
			return m_ExitCode;
		}

		Start();
		if (m_ExitCode)
			return m_ExitCode;

		while (!m_pEngine->IsExiting())
			m_pEngine->RunFrame();

		Stop();

		return m_ExitCode;
	}

	void Application::ErrorExit(const std::string& message)
	{
		// engine exit
		m_ExitCode = EXIT_FAILURE;

		// 로그에 출력해야 하는데 현 시점에선 로거가 만들어지지 않았다.
		// 즉, Engine과는 다른 시점에서 생성되어야 한다.
		if (message.empty())
		{
			// typename과 예상치 에러로 어플리케이션 동작이 종료되었습니다.
		}
		else
		{
			// typename과 메시지를 출력
		}
	}
}