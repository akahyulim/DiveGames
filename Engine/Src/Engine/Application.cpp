#include "divepch.h"
#include "Application.h"
#include "Base/Engine.h"

namespace Dive
{
	Application::Application()
		: m_ExitCode(EXIT_SUCCESS)
	{
		// engine parameters: �̰� ���� ���� �𸣰ڴ�.

		m_pEngine = std::make_shared<Engine>();

	}

	int Application::Run()
	{
		Setup();
		if (m_ExitCode)
			return m_ExitCode;

		// engine intialize: ���� �Ķ���͸� ����
		{
			ErrorExit();
			return m_ExitCode;
		}

		Start();
		if (m_ExitCode)
			return m_ExitCode;

		// while engine runframe: ������ ���� ���� �����ʿ��� �����Ǿ�� �Ѵ�.
		// �ƹ����� Graphics���� ���� �� ������ �ϴ� �� ����.

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