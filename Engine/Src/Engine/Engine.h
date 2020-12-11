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
		
		const Context* GetContext() const { return m_context; }

		// �ᱹ �̰� ������ SystemManager�� this pointer�� ������ �ȴ�.
		const WindowData& GetWindowData()			const { return m_windowData; }

		template<class T> std::shared_ptr<T> GetSubsystem()
		{
			if (!m_context)
				return nullptr;

			return m_context->GetSubsystem<T>();
		}

	private:
		WindowData m_windowData;
		Context* m_context;

		bool m_bInitialized;
		bool m_bExiting;
	};
}

