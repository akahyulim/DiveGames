#include "divepch.h"
#include "Application.h"
#include "DvEngine.h"
#include "Core/DvEventSystem.h"

namespace Dive
{
	Application::Application(DvContext* pContext)
		: DvObject(pContext),
		m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters
		// 엔진 파라미터의 디폴트 key와 value를 얻는 것 같다.
		// Setup()에서 따로 설정되지 않으면 이를 바로 Engine::Initialize()에 전달한다.

		m_pEngine = new DvEngine(pContext);

		// 최초 이벤트 구독은 에러 발생시 로그를 구성하는 것이다.
		// 해당 로그는 ErrorExit()를 통해 출력되지만, 정작 ErrorExit()를 호출하는 구문이 엔진 초기화 빼곤 없다...
		DV_SUBSCRIBE_TO_EVENT(eDvEventType::LogMessage, DV_EVENT_HANDLER_VARIANT(OnLogMessage));
	}

	Application::~Application()
	{
		// shared_ptr로 바꾸기?
		if (m_pEngine)
		{
			delete m_pEngine;
			m_pEngine = nullptr;
		}
	}

	int Application::Run()
	{
		Setup();
		if (m_ExitCode)
			return m_ExitCode;

		if(!m_pEngine->Initialize(m_EngineParameters))
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

	// Setup, Start에서 호출될 수 있다.
	// Engine 초기화 과정 중 E_LOGMESSAGE가 발생하면
	// 역시 바로 호출된다.
	void Application::ErrorExit(const std::string& message)
	{
		m_pEngine->Exit();
		m_ExitCode = EXIT_FAILURE;

		// 둘 다 메시지 박스에 출력된다.
		if (message.empty())
		{
			// m_StartupErrors를 출력
		}
		else
		{
			// message를 출력
		}
	}

	void Application::OnLogMessage(const Variant& data)
	{
		// Logger가 로그를 작성할 때 마다 발생하는 이벤트의 콜백 함수다.
		// Error 메시지만 추려 m_StartupError에 저장한다.

		std::cout << "이벤트 받음 - " << data.Get<std::string>().c_str() << std::endl;
	}
}