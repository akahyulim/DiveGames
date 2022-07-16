#include "divepch.h"
#include "Application.h"
#include "DvEngine.h"

namespace Dive
{
	Application::Application(DvContext* pContext)
		: DvObject(pContext),
		m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters: �̰� ���� ���� �𸣰ڴ�.

		m_pEngine = new DvEngine(pContext);

	}

	int Application::Run()
	{
		Setup();
		if (m_ExitCode)
			return m_ExitCode;

		// �Ķ���͸� �Ű����ڷ� �����ؾ� �Ѵ�.
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

		// �α׿� ����ؾ� �ϴµ� �� �������� �ΰŰ� ��������� �ʾҴ�.
		// ��, Engine���� �ٸ� �������� �����Ǿ�� �Ѵ�.
		if (message.empty())
		{
			// typename�� ����ġ ������ ���ø����̼� ������ ����Ǿ����ϴ�.
		}
		else
		{
			// typename�� �޽����� ���
		}
	}
}