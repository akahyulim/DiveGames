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

		SystemManager* GetSystemManager() const { return m_pSystemManager.get(); }

	private:
		std::shared_ptr<SystemManager> m_pSystemManager;
	};
}

