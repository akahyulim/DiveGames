#pragma once

namespace Dive
{
	class Engine
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void RunFrame();
		static void Update();
		static void Render();

		static void Exit();

		static bool IsExiting();
	};
}