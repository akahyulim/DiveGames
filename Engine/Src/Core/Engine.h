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
		Engine();
		Engine(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed);
		~Engine();

		void Update();

		bool IsInitialized() const { return m_bInitialized; }
		
		const std::shared_ptr<SystemManager>& GetSystemManager()	const { return m_systemManager; }

		// 결국 이것 때문에 SystemManager가 this pointer를 가지게 된다.
		const WindowData& GetWindowData()							const { return m_windowData; }

	private:
		WindowData m_windowData;
		std::shared_ptr<SystemManager> m_systemManager;

		bool m_bInitialized = false;
	};
}

