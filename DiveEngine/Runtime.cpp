#include "Runtime.h"


namespace Dive
{
	void Runtime::Initialize()
	{
		if (m_bInitialized)
		{
			return;
		}
		m_bInitialized = true;

		// sub system »ý¼º
	}

	void Runtime::Run()
	{
		if (!m_bInitialized)
		{
			Initialize();
		}

		m_deltaTime = static_cast<float>(max(0.0, m_timer.GetElapsedTime() / 1000.0f));
		m_timer.Record();

		if (m_bWindowActive)
		{
			Update(m_deltaTime);
			Render();
		}
		else
		{

		}
	}
	
	void Runtime::Update(float deltaTime)
	{
	}
	
	void Runtime::FixedUpdate()
	{
	}
	
	void Runtime::Render()
	{
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
	}
}