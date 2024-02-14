#pragma once

namespace Dive
{
	class Application
	{
	public:
		Application(const std::wstring title = L"DIVE", uint32_t windowWidth = 800, uint32_t windowHeight = 600, bool fullscreen = false);
		virtual ~Application();

		bool InitializeWindow();
		
		int Run();

		virtual void OnSetup() {}
		virtual void OnStart() {}
		virtual void OnStop() {}

		 uint32_t GetWindowWidth() const { return m_WindowWidth; }
		 uint32_t GetWindowHeight() const { return m_WindowHeight; }
		 DirectX::XMFLOAT2 GetWindowSize() const { return { (float)m_WindowWidth, (float)m_WindowHeight }; }

		 std::wstring GetTitle() const { return m_Title; }
		void SetTitle(const std::wstring& title);

		 static Application& GetApplication() { return *s_pInstance; }

	private:
		std::wstring m_Title;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		bool m_bFullscreen;
		bool m_bRunning;

		static Application* s_pInstance;
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
	Dive::Log::Initialize();	\
	auto pApplication = std::make_unique<className>();	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())