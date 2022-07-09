#include "divepch.h"
#include "Application.h"
#include "Base/Engine.h"

namespace Dive
{
	Application::Application()
		: m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters: 이건 아직 뭔지 모르겠다.

		m_pEngine = std::make_shared<Engine>();

	}

	int Application::Run()
	{
		Setup();
		if (m_ExitCode)
			return m_ExitCode;

		// engine intialize: 엔진 파라미터를 전달
		{
			ErrorExit();
			return m_ExitCode;
		}

		Start();
		if (m_ExitCode)
			return m_ExitCode;

		// while engine runframe: 윈도우 생성 역시 엔진쪽에서 구현되어야 한다.
		// 아무래도 Graphics에서 생성 및 관리를 하는 것 같다.

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