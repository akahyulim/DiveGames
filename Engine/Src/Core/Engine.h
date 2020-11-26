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

		bool IsInitialized() const { return m_bInitialized; }
		
		const std::shared_ptr<SystemManager>& GetSystemManager()	const { return m_SystemManager; }

		// �ᱹ �̰� ������ SystemManager�� this pointer�� ������ �ȴ�.
		const WindowData& GetWindowData()							const { return m_WindowData; }

	private:
		WindowData m_WindowData;
		std::shared_ptr<SystemManager> m_SystemManager;

		bool m_bInitialized = false;
	};
}

