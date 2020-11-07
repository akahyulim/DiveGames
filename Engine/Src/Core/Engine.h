#pragma once


namespace Dive
{
	class SystemManager;

	struct WindowData
	{
		HWND hWnd;
		int ResolutionWidth;
		int ResolutionHeight;
		bool bVSync;
		bool bWindowed;
	};

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Initialize(const WindowData& windowData);
		void Update();

		SystemManager* GetSystemManager() const { return m_SystemManager.get(); }

		const WindowData& GetWindowData() const { return m_WindowData; }

	private:
		WindowData m_WindowData;
		std::shared_ptr<SystemManager> m_SystemManager;
	};
}

