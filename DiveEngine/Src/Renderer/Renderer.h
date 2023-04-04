#pragma once

namespace Dive
{
	class View;

	class Renderer
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Update(float delta);
		static void Render();

		static void SetView(uint32_t index, View* pView);

	private:
		static bool createShaders();
	};
}