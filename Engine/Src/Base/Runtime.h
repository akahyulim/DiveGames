#pragma once
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Settings.h"

namespace Dive
{
	class SceneManager;
	class ResourceManager;
	class RenderPath;

	class Runtime
	{
	public:
		Runtime(const std::string& name = "Dive App");
		virtual ~Runtime();

		virtual bool Initialize();

		void Run();

		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Compose();

		void ActivatePath(RenderPath* pPath);
		RenderPath* GetActivePath() { return m_pActivePath; }

		static Runtime& Get() { return *s_pInstance; }

		HWND GetWindowHandle() { return m_hWnd; }
		void SetWindow(HWND windowHandle, bool fullScreen);

		float GetTargetFrameRate() const { return m_TargetFrameRate; }
		void SetTargetFrameRate(float fps) { m_TargetFrameRate = fps; }

		bool IsFrameLock() const { return m_bFrameLock; }
		void SetFrameLock(bool enable) { m_bFrameLock = enable; }

		void ActiveWindow(bool active) { m_bActiveWindow = active; }

		bool IsInitialized() const { return m_bInitialized; }

		std::string GetIniFilePath() const { return m_IniFilePath; }


		virtual void ModifyWindow(eWindowModes mode, unsigned int width, unsigned int height, bool maximize);

		void OnResizeResolution(unsigned int data);

	protected:

	protected:
		static Runtime* s_pInstance;

		std::string m_AppName;

		HWND m_hWnd = 0;
		bool m_bFullScreen = false;

		RenderPath* m_pActivePath = nullptr;

		// 이 두 값은 Runtime이 관리하는 게 맞는 듯?
		float m_TargetFrameRate = 60.0f;
		bool m_bFrameLock = false;

		bool m_bInitialized = false;
		bool m_bActiveWindow = true;

		std::string m_IniFilePath;
	};
}