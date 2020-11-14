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
	};

	class Engine
	{
	public:
		Engine(HINSTANCE hInstance, HWND hWnd, int width, int height);
		~Engine();

		void Update();

		SystemManager* GetSystemManager() const { return m_SystemManager.get(); }
		const WindowData& GetWindowData() const { return m_WindowData; }

	private:
		WindowData m_WindowData;
		std::shared_ptr<SystemManager> m_SystemManager;
	};
}

