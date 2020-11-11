#pragma once


namespace Dive
{
	class SystemManager;

	// 최소한 핸들과 윈도우 모드 정보는 필요하다.
	// 게다가 윈도우 모드 정보를 갱신될 수 있다....
	// input에 윈도우 인스턴스도 필요하다. 게다가 메시지까지 저장해놨다...
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

