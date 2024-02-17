#pragma once

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		Application();
		virtual ~Application();

		int Run();

		virtual void OnSetup() {}
		virtual void OnStart() {}
		virtual void OnStop() {}

		uint32_t GetWindowWidth() const { return m_WindowWidth; }
		uint32_t GetWindowHeight() const { return m_WindowHeight; }
		DirectX::XMFLOAT2 GetWindowSize() const { return { (float)m_WindowWidth, (float)m_WindowHeight }; }

	protected:
		std::wstring m_Title;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		bool m_bFullscreen;

		std::unique_ptr<Engine> m_pEngine;
	};
}

#define DEFINE_MAIN(function)	\
int main()	\
{	\
	return function;	\
}

#define DEFINE_APPLICATION_MAIN(className)	\
int RunApplication()	\
{	\
	auto pApplication = std::make_unique<className>();	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())