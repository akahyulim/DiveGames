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
		m_pEngine = std::make_unique<Engine>(pContext);

		SUBSCRIBE_EVENT(eEventType::LogMessage, EVENT_HANDLER_PARAM(OnLogMessage));
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