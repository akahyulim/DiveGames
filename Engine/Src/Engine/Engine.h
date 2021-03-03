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

	// 이름을 Runtime으로 바꾸는 편이 나을 것 같다.
	// Editor는 이를 상속받는다. 따라서 protected로 멤버 변수를 관리하자.
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
		
		Context* GetContext() const { return m_context; }

		// 결국 이것 때문에 SystemManager가 this pointer를 가지게 된다.
		const WindowData& GetWindowData()			const { return m_windowData; }

		template<class T> std::shared_ptr<T> GetSubsystem()
		{
			if (!m_context)
				return nullptr;

			return m_context->GetSubsystem<T>();
		}

	private:
		Engine(const Engine&)				= delete;
		Engine& operator=(const Engine&)	= delete;

	private:
		WindowData m_windowData;
		Context* m_context;

		bool m_bInitialized;
		bool m_bExiting;
	};
}

