#pragma once
#include "Src/Renderer/Graphics/GraphicsDevice.h"
#include "Settings.h"
#include <Windows.h>
#include <string>

namespace dive
{
	class SceneManager;
	class AssetManager;
	class RenderPath;

	// m_bLockFrame�� VSync�͵� �����ִ�.

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

		void ActivatePath(RenderPath* pPath);
		RenderPath* GetActivePath() { return m_pActivePath; }

		HWND GetWindowHandle() { return m_hWnd; }
		void SetWindow(HWND windowHandle, bool fullScreen);

		float GetTargetFrame() const { return m_TargetFPS; }
		void SetTargetFrame(float fps) { m_TargetFPS = fps; }

		bool IsFrameLock() const { return m_bFrameLock; }
		void SetFrameLock(bool enable) { m_bFrameLock = enable; }

		void ActiveWindow(bool active) { m_bActiveWindow = active; }

		bool IsInitialized() const { return m_bInitialized; }

		std::string GetIniFilePath() const { return m_IniFilePath; }


		virtual void ModifyWindow(eWindowModes mode, unsigned int width, unsigned int height, bool maximize);

		void OnResizeResolution(unsigned int data);

		// get systems

	protected:

	protected:
		std::string m_AppTitle;

		HWND m_hWnd = 0;
		bool m_bFullScreen = false;

		RenderPath* m_pActivePath = nullptr;

		float m_DeltaTime = 0.0f;
		float m_TargetFPS = 60.0f;
		bool m_bFrameLock = false;

		bool m_bInitialized = false;
		bool m_bActiveWindow = true;

		std::string m_IniFilePath;
	};
}