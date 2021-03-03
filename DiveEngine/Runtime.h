#pragma once
#include "Timer.h"

#include <Windows.h>

namespace Dive
{
	class Runtime
	{
	public:
		virtual void Initialize();

		void Run();

		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render();

		void SetWindow(HWND windowHandle, bool fullScreen);

	protected:
		Dive_Timer m_timer;

		float m_deltaTime = 0.0f;

		bool m_bInitialized = false;
		bool m_bWindowActive = false;
	};
}