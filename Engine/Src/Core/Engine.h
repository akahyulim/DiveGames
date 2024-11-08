#pragma once

namespace Dive
{
	class Window;

	class DvEngine
	{
	public:

		//static bool Initialize();
		//static void Release();

		static Window* GetAppWindow();
		static void SetAppWindow(Window* pWindow);

		static LRESULT MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
	private:
		static Window* m_pAppWindow;
	};

	class Engine
	{
	public:
		Engine(const Engine&) = delete;
		void operator=(const Engine&) = delete;

		static Engine* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Engine;

			return s_pInstance;
		}

		bool Initialize(uint32_t width, uint32_t height, bool fullScreen);
		void Shutdown();

		void RunFrame();
		void OnUpdate();
		void OnRender();

		bool IsInitialized();
		bool IsExit();

		double GetFps();

		double GetTimeMS();
		double GetTimeSec();

		double GetDeltaTimeMS();
		double GetDeltaTimeSec();

	private:
		Engine();
		~Engine() = default;

	private:
		static Engine* s_pInstance;

		bool m_bInitialized;
		bool m_bExit;

		double m_TimeMS;
		double m_DeltaTimeMS;

		std::chrono::steady_clock::time_point m_LastTickTime;

		int m_FrameCount;
		double m_LastTime;
		double m_Fps;
	};

	Engine* GetEngine();
}