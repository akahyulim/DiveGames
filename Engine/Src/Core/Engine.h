#pragma once


namespace Dive
{
	class SystemManager;

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Initialize(HINSTANCE hInstance, HWND hWnd);
		void Update();

		SystemManager* GetSystemManager() const { return m_SystemManager.get(); }

	private:
		std::shared_ptr<SystemManager> m_SystemManager;
	};
}

