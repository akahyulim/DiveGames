#pragma once
#include <entt/entt.hpp>

namespace Dive
{
	class RenderTexture;

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void RenderScene(const entt::registry& registry);
		
		static void Present();

		static void ResizeRenderTargets(UINT32 width, UINT32 height);
		static RenderTexture* GetRenderTarget() { return s_RenderTarget.get(); }

	private:

	private:
		static UINT32 s_Width;
		static UINT32 s_Height;

		static std::unique_ptr<RenderTexture> s_RenderTarget;
	};
}
