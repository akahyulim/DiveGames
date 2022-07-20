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
		// ���� �Ķ������ ����Ʈ key�� value�� ��� �� ����.
		// Setup()���� ���� �������� ������ �̸� �ٷ� Engine::Initialize()�� �����Ѵ�.

		m_pEngine = new DvEngine(pContext);

		// ���� �̺�Ʈ ������ ���� �߻��� �α׸� �����ϴ� ���̴�.
		// �ش� �α״� ErrorExit()�� ���� ��µ�����, ���� ErrorExit()�� ȣ���ϴ� ������ ���� �ʱ�ȭ ���� ����...
		DV_SUBSCRIBE_TO_EVENT(eDvEventType::LogMessage, DV_EVENT_HANDLER_VARIANT(OnLogMessage));
	}

	Application::~Application()
	{
		// shared_ptr�� �ٲٱ�?
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

	// Setup, Start���� ȣ��� �� �ִ�.
	// Engine �ʱ�ȭ ���� �� E_LOGMESSAGE�� �߻��ϸ�
	// ���� �ٷ� ȣ��ȴ�.
	void Application::ErrorExit(const std::string& message)
	{
		m_pEngine->Exit();
		m_ExitCode = EXIT_FAILURE;

		// �� �� �޽��� �ڽ��� ��µȴ�.
		if (message.empty())
		{
			// m_StartupErrors�� ���
		}
		else
		{
			// message�� ���
		}
	}

	void Application::OnLogMessage(const Variant& data)
	{
		// Logger�� �α׸� �ۼ��� �� ���� �߻��ϴ� �̺�Ʈ�� �ݹ� �Լ���.
		// Error �޽����� �߷� m_StartupError�� �����Ѵ�.

		std::cout << "�̺�Ʈ ���� - " << data.Get<std::string>().c_str() << std::endl;
	}
}