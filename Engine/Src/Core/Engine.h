#pragma once

namespace Dive
{
	class Window;
	class LogManager;
	class DvGraphics;
	//class Scene;

	extern class DvEngine* GEngine;
	extern class Scene* GWorld;

	class DvEngine
	{
	public:
		static DvEngine* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new DvEngine;

			return s_pInstance;
		}

		bool Initialize(bool fullScreen = false);
		bool Initialize(const std::wstring& title, uint32_t width, uint32_t height, bool fullScreen = false);

		bool IsRun();

		LRESULT MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void Draw();
		void Present();

		Scene* GetWorldByName(const std::string& name) const;

		void AddWorld(Scene* pWorld);
		void RemoveWorld(Scene* pWorld);

		Window& GetWindow() const { return *m_pAppWindow; }
		DvGraphics& GetGraphics() const { return *m_pGraphics; }

	private:
		DvEngine();
		~DvEngine();

	private:
		static DvEngine* s_pInstance;
		
		std::unique_ptr<Window> m_pAppWindow;
		std::unique_ptr<DvGraphics> m_pGraphics;

		std::vector<Scene*> m_Worlds;
	};

	//============================================================================================================================================

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