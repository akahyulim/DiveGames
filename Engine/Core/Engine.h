#pragma once


namespace Dive
{
	class SystemManager;

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Initialize();
		void Update();

		SystemManager* GetSystemManager() const { return m_pSystemManager; }

	private:
		SystemManager* m_pSystemManager = nullptr;
	};
}

