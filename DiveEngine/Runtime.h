#pragma once
#include "GraphicsDevice.h"
#include <Windows.h>
#include <string>

namespace Dive
{
	class AssetManager;
	class RenderPath;

	// m_bLockFrame은 VSync와도 연관있다.

	class Runtime
	{
	public:
		Runtime();
		virtual ~Runtime();

		virtual bool Initialize();

		void Run();

		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Compose();

		void ActivatePath(RenderPath* path);
		RenderPath* GetActivePath() { return m_pActivePath; }

		void SetWindow(HWND windowHandle, bool fullScreen);

		float GetTargetFrame() const { return m_targetFPS; }
		void SetTargetFrame(float fps) { m_targetFPS = fps; }

		bool IsFrameLock() const { return m_bFrameLock; }
		void SetFrameLock(bool enable) { m_bFrameLock = enable; }

		void ActiveWindow(bool active) { m_bWindowActive = active; }

		bool IsInitialized() const { return m_bInitialized; }

		std::string GetIniFilePath() const { return m_iniFilePath; }

		AssetManager* GetAssetManager() { return m_pAssetManager; }

	protected:
		HWND m_hWnd = 0;
		bool m_bFullScreen = false;

		RenderPath* m_pActivePath = nullptr;

		float m_deltaTime = 0.0f;
		float m_targetFPS = 60.0f;
		bool m_bFrameLock = false;

		bool m_bInitialized = false;
		bool m_bWindowActive = true;

		std::string m_iniFilePath;

		AssetManager* m_pAssetManager;
	};
}