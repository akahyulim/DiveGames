#include "stdafx.h"
#include "Application.h"

namespace Dive
{
	Application::Application()
	{
		m_engine = std::make_unique<Engine>();
	}

	int Dive::Application::Run()
	{
		Setup();

		if (!m_engine->Initialize())
		{
			m_engine->DoExit();
			return -1;
		}

		Start();

		while (!m_engine->IsExit())
			m_engine->RunFrame();

		Stop();

		return 0;
	}
}