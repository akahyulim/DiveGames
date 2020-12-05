#pragma once
#include "Core/Dive_Object.h"


namespace Dive
{
	class Dive_Context;
	class Window;

	class Graphics : public Dive_Object
	{
		NEW_DIVE_OBJECT(Graphics);

	public:
		Graphics(Dive_Context* context);
		~Graphics();

		bool BeginFrame();
		void EndFrame();

		// test
		void Update(size_t eventType);

		bool IsInitialized();

		// settings를 얻어와야 한다.
		bool SetScreenMode();

		std::shared_ptr<Window> GetWindow() const { return m_window; }

	private:
	private:
		std::shared_ptr<Window> m_window;
	};
}