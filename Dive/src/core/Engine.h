#pragma once

namespace Dive
{
	enum class eEngineMode
	{
		Editor,
		Runtime,
	};

	class Engine
	{
	public:
		Engine() = default;
		~Engine() = default;

		bool Initialize();
		void Shutdown();

		void RunFrame();

		bool IsExit() { return m_bExit; }
		void DoExit();

	private:
		bool m_bExit = false;
	};
}
