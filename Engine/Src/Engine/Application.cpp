#include "divepch.h"
#include "Application.h"
#include "DvEngine.h"
#include "Core/DvEventSystem.h"
#include "Base/Base.h"	// core�� �ű��?

#include <sstream>

namespace Dive
{
	Application::Application(DvContext* pContext)
		: DvObject(pContext),
		m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters
		// ���� �Ķ������ ����Ʈ key�� value�� ��� �� ����.
		// Setup()���� ���� �������� ������ �̸� �ٷ� Engine::Initialize()�� �����Ѵ�.

		m_pEngine = std::make_shared<DvEngine>(pContext);
		m_pContext->RegisterSubsystem(m_pEngine);

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::LogMessage, DV_EVENT_HANDLER_VARIANT(OnLogMessage));
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

		if (message.empty())
		{
			MessageBoxA(nullptr, message.c_str(), "ERROR", MB_OK);
		}
		else
		{
			MessageBoxA(nullptr, 
				m_ErrorMessage.empty() ? "�� �� ���� ������ �����մϴ�." : m_ErrorMessage.c_str(), 
				"ERROR", MB_OK);
		}
	}

	void Application::OnLogMessage(const Variant& data)
	{
		auto logMessage = data.Get<std::string>();

		std::stringstream ss;
		ss.str(logMessage);

		std::string level;
		ss >> level;

		if (ss.str() == "error")
		{
			m_ErrorMessage = logMessage.substr(logMessage.find_first_of(' ') + 1);
		}
	}
}