#include "divepch.h"
#include "Application.h"
#include "Engine.h"
#include "Core/EventSystem.h"
#include "Core/CoreDefs.h"
#include "IO/IOEvents.h"

#include <sstream>

namespace Dive
{
	Application::Application(Context* pContext)
		: Object(pContext),
		m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters
		// 엔진 파라미터의 디폴트 key와 value를 얻는 것 같다.
		// Setup()에서 따로 설정되지 않으면 이를 바로 Engine::Initialize()에 전달한다.

		m_pEngine = std::make_shared<Engine>(pContext);
		m_pContext->RegisterSubsystem(m_pEngine);

		SUBSCRIBE_EVENT(eEventType::LogMessage, EVENT_HANDLER(OnLogMessage));
	}

	Application::~Application()
	{
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

	void Application::ErrorExit(const std::string& message)
	{
		m_pEngine->Exit();
		m_ExitCode = EXIT_FAILURE;

		if (!message.empty())
		{
			MessageBoxA(nullptr, message.c_str(), "ERROR", MB_OK);
		}
		else
		{
			MessageBoxA(nullptr, 
				m_ErrorMessage.empty() ? "알 수 없는 오류로 종료합니다." : m_ErrorMessage.c_str(), 
				"ERROR", MB_OK);
		}
	}

	void Application::OnLogMessage(const Event& e)
	{
		auto& evnt = dynamic_cast<const LogMessageEvent&>(e);

		unsigned int levelErr = 4;

		if (evnt.GetLogLevel() == levelErr)
		{
			m_ErrorMessage.clear();
			m_ErrorMessage = evnt.GetLogMessage();
		}
	}
}