#pragma once

namespace Dive
{
	class Engine
	{
	public:
		static bool Initialize(uint32_t width, uint32_t height, bool fullScreen);
		static void Shutdown();

		static void RunFrame();
		static void OnUpdate();
		static void OnRender();

		static bool IsInitialized();
		static bool IsExit();
	};
}