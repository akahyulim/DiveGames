#pragma once

namespace Dive
{
	class RenderTexture;

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Update();
		static void Render();
		static void Tick();

		static void ResizeRenderTargets(uint32_t width, uint32_t height);
		static RenderTexture* GetRenderTarget() { return s_RenderTarget.get(); }

	private:
		static uint64_t s_FrameCount;

		static uint32_t s_Width;
		static uint32_t s_Height;

		static std::unique_ptr<RenderTexture> s_RenderTarget;
	};
}