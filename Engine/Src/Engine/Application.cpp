#include "divepch.h"
#include "Application.h"
#include "Engine.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Core/CoreDefs.h"
#include "IO/IOEvents.h"
#include "IO/Log.h"

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

		if(!m_pEngine->Initialize(m_EngineParams))
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

	// 특정 로그래벨만 에러 메시지를 저장한다?
	void Application::OnLogMessage(const Event& e)
	{
		auto& evnt = dynamic_cast<const LogMessageEvent&>(e);
		if (evnt.GetLogLevel() == static_cast<uint32_t>(eLogLevels::Error))
		{
			m_ErrorMessage.clear();
			m_ErrorMessage = evnt.GetLogMessage();
		}
	}
}