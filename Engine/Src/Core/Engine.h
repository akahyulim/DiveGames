#pragma once

namespace Dive
{
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