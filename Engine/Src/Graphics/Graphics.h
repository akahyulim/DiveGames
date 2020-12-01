#pragma once
#include "Core/Subsystem.h"


namespace Dive
{
	class Context;
	class Window;

	class Graphics : public Subsystem
	{
	public:
		Graphics(Context* context);
		~Graphics();

		bool BeginFrame();
		void EndFrame();

		bool IsInitialized();

		// settings를 얻어와야 한다.
		bool SetScreenMode();

	private:
	private:
		std::shared_ptr<Window> m_window;
	};
}