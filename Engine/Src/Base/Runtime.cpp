#include "divepch.h"
#include "Runtime.h"
#include "DiveCore.h"
#include "Time.h"
#include "Log.h"
#include "AppWindow.h"

namespace Dive
{
	Runtime* Runtime::s_pInstance = nullptr;

	Runtime::Runtime(HINSTANCE hInstance, const std::string& title)
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		Log::Initialize();
		Time::Initialize();

		m_pAppWindow = std::make_unique<AppWindow>(hInstance, title);

	}

	Runtime::~Runtime()
	{
		m_pAppWindow->Destroy();
	}

	void Runtime::Run()
	{
		Time::Update();

		if (m_pAppWindow)
		{
			while (m_pAppWindow->Run())
			{

			}
		}
	}
}