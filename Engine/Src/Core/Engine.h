#pragma once

namespace Dive
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Initialize(uint32_t width, uint32_t height, bool fullScreen);
		void RunFrame();
		void OnUpdate();
		void OnRender();

		bool IsInitialized() const { return m_bInitialized; }
		bool IsExit() const { return m_bExit; }

	private:
		bool m_bInitialized;
		bool m_bExit;
	};
}