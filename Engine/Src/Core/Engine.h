#pragma once

namespace Dive
{
	// Application이 존재하는데 이게 굳이 필요있나 싶다.
	class Engine
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Tick();

		static void Exit();

		static bool IsExiting();
	};
}