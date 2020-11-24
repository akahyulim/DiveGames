#pragma once


namespace Dive
{
	class SystemManager;

	struct WindowData
	{
		HINSTANCE hInstance;
		HWND hWnd;
		int width;
		int height;
		bool windowed;
	};

	class Engine
	{
	public:
		Engine(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed);
		~Engine();

		void Update();

		bool IsInitialized()				const { return m_bInitialize; }
		SystemManager* GetSystemManager()	const { return m_SystemManager.get(); }
		const WindowData& GetWindowData()	const { return m_WindowData; }

	private:
		WindowData m_WindowData;
		std::shared_ptr<SystemManager> m_SystemManager;

		bool m_bInitialize = false;
	};
}

