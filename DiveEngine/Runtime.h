#pragma once
#include "Timer.h"
#include "GraphicsDevice.h"
#include <Windows.h>

namespace Dive
{
	class RenderPath;

	class Runtime
	{
	public:
		Runtime();
		virtual ~Runtime() = default;

		virtual void Initialize();

		void Run();

		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Compose(CommandList cmd);

		void SetActivePath(RenderPath* path);
		RenderPath* GetActivePath() { return m_activePath; }

		void SetWindow(HWND windowHandle, bool fullScreen);

	protected:
		Timer m_timer;
		RenderPath* m_activePath = nullptr;

		float m_deltaTime = 0.0f;

		bool m_bInitialized = false;
		bool m_bWindowActive = false;
	};
}