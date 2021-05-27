#pragma once
#include "GraphicsDevice.h"
#include <Windows.h>

namespace Dive
{
	class RenderPath;

	// m_bLockFrame은 VSync와도 연관있다.

	class Runtime
	{
	public:
		Runtime();
		virtual ~Runtime() = default;

		virtual bool Initialize();

		void Run();

		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Compose();

		void ActivatePath(RenderPath* path);
		RenderPath* GetActivePath() { return m_activePath; }

		void SetWindow(HWND windowHandle, bool fullScreen);

		float GetTargetFrame() const { return m_targetFPS; }
		void SetTargetFrame(float fps) { m_targetFPS = fps; }

		bool IsFrameLock() const { return m_bFrameLock; }
		void SetFrameLock(bool enable) { m_bFrameLock = enable; }

		void ActiveWindow(bool active) { m_bWindowActive = active; }

		bool IsInitialized() const { return m_bInitialized; }

	protected:
		HWND m_hWnd = 0;
		bool m_bFullScreen = false;

		RenderPath* m_activePath = nullptr;

		float m_deltaTime = 0.0f;
		float m_targetFPS = 60.0f;
		bool m_bFrameLock = false;

		bool m_bInitialized = false;
		bool m_bWindowActive = true;
	};
}