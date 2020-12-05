#pragma once


namespace Dive
{
	class Dive_Context;

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
		Engine();
		~Engine();

		// setting data를 넘겨줘야 한다.
		bool Initialize();
		void DoExit();

		void RunFrame();

		// 사실 애네 둘은 private이어야 한다.
		void Update();
		void Render();

		bool IsInitialized()	const { return m_bInitialized; }
		bool IsExiting()		const { return m_bExiting; }
		
		const Dive_Context* GetContext() const { return m_dive_context; }

		// 결국 이것 때문에 SystemManager가 this pointer를 가지게 된다.
		const WindowData& GetWindowData()			const { return m_windowData; }

	private:
		WindowData m_windowData;
		Dive_Context* m_dive_context;

		bool m_bInitialized;
		bool m_bExiting;
	};
}

