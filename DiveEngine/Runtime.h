#pragma once
#include "GraphicsDevice.h"
#include "Settings.h"
#include <Windows.h>
#include <string>

namespace dive
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
		RenderPath* GetActivePath() { return mActivePath; }

		HWND GetWindowHandle() { return mWindowHandle; }
		void SetWindow(HWND windowHandle, bool fullScreen);

		float GetTargetFrame() const { return mTargetFPS; }
		void SetTargetFrame(float fps) { mTargetFPS = fps; }

		bool IsFrameLock() const { return mbFrameLock; }
		void SetFrameLock(bool enable) { mbFrameLock = enable; }

		void ActiveWindow(bool active) { mbActiveWindow = active; }

		bool IsInitialized() const { return mbInitialized; }

		std::string GetIniFilePath() const { return mIniFilePath; }

		AssetManager* GetAssetManager() { return mAssetManager; }

		virtual void ModifyWindow(eWindowModes mode, unsigned int width, unsigned int height, bool maximize);

		void OnResizeResolution(unsigned int data);

	protected:

	protected:
		std::string mAppTitle;

		HWND mWindowHandle = 0;
		bool mbFullScreen = false;

		RenderPath* mActivePath = nullptr;

		float mDeltaTime = 0.0f;
		float mTargetFPS = 60.0f;
		bool mbFrameLock = false;

		bool mbInitialized = false;
		bool mbActiveWindow = true;

		std::string mIniFilePath;

		AssetManager* mAssetManager;
	};
}