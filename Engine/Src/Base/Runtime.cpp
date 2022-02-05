#include "divepch.h"
#include "Runtime.h"
#include "DiveCore.h"
#include "Time.h"
#include "Log.h"
#include "AppWindow.h"

namespace Dive
{
	Runtime* Runtime::s_pInstance = nullptr;

	Runtime::Runtime()
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		Log::Initialize();
		Time::Initialize();

		m_pAppWindow = new AppWindow;
	}

	Runtime::~Runtime()
	{
		DV_DELETE(m_pAppWindow);
	}

	void Runtime::Initialize()
	{
		m_pAppWindow->Create();
	}

	void Runtime::Run()
	{
		if (m_pAppWindow)
		{
			while (m_pAppWindow->Run())
			{

			}
		}
	}
}