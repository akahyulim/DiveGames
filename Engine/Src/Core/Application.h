#pragma once

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		int Run();

		virtual void OnSetup() {}
		virtual void OnStart() {}
		virtual void OnStop() {}

		uint32_t GetWindowWidth() const { return m_WindowWidth; }
		uint32_t GetWindowHeight() const { return m_WindowHeight; }
		DirectX::XMFLOAT2 GetWindowSize() const { return { (float)m_WindowWidth, (float)m_WindowHeight }; }

	protected:
		Engine* m_pEngine;

		std::wstring m_Title;
		uint32_t m_WindowWidth;			// 윈도우 크기라 명명하면 어울리지 않는다.
		uint32_t m_WindowHeight;
		bool m_bFullscreen;
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