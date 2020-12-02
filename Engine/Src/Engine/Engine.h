#pragma once


namespace Dive
{
	class Context;

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

		// setting data�� �Ѱ���� �Ѵ�.
		bool Initialize();
		void DoExit();

		void RunFrame();

		// ��� �ֳ� ���� private�̾�� �Ѵ�.
		void Update();
		void Render();

		bool IsInitialized()	const { return m_bInitialized; }
		bool IsExiting()		const { return m_bExiting; }
		
		const std::shared_ptr<Context>& GetContext() const { return m_context; }

		// �ᱹ �̰� ������ SystemManager�� this pointer�� ������ �ȴ�.
		const WindowData& GetWindowData()							const { return m_windowData; }

	private:
		WindowData m_windowData;
		std::shared_ptr<Context> m_context;

		bool m_bInitialized;
		bool m_bExiting;
	};
}

