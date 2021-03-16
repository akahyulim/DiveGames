#pragma once
#include "GraphicsDevice.h"
#include <Windows.h>

namespace Dive
{
	class RenderPath;

	// m_bWindowActive�� ������ ���¿��� ���;� �Ѵ�.
	// m_bLockFrame�� VSync�͵� �����ִ�.

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

		float GetTargetFrame() const { return m_targetFPS; }
		void SetTargetFrame(float fps) { m_targetFPS = fps; }

		bool IsFrameLock() const { return m_bFrameLock; }
		void SetFrameLock(bool enable) { m_bFrameLock = enable; }

	protected:
		RenderPath* m_activePath = nullptr;

		float m_deltaTime = 0.0f;
		float m_targetFPS = 60.0f;
		bool m_bFrameLock = false;

		bool m_bInitialized = false;
		bool m_bWindowActive = true;
	};
}