#include "stdafx.h"
#include "Renderer.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "graphics/Graphics.h"
#include "graphics/RenderTexture.h"
#include "world/WorldManager.h"
#include "world/World.h"
#include "world/GameObject.h"
#include "world/Components/Camera.h"
#include "world/Components/Renderable.h"

namespace Dive
{
	uint64_t Renderer::s_FrameCount = 0;
	uint32_t Renderer::s_Width = 1280;
	uint32_t Renderer::s_Height = 760;
	std::unique_ptr<RenderTexture> Renderer::s_RenderTarget;

	void Renderer::Initialize()
	{
		Shutdown();

		// 각종 리소스 생성(렌더타겟, states)

		s_RenderTarget = std::make_unique<RenderTexture>(s_Width, s_Height);
	}
	
	void Renderer::Shutdown()
	{

		// 생성한 리소스 릴리즈
	}

	void Renderer::Update()
	{
		auto activeWorld = WorldManager::GetActiveWorld();
		if (!activeWorld)
			return;
	}

	void Renderer::Render()
	{
		// 렌더링 패스 수행

		DV_FIRE_EVENT(eEventType::PostRender);

		Graphics::Present();
	}

	void Renderer::Tick()
	{
		Update();
		Render();

		s_FrameCount++;
	}

	void Renderer::ResizeRenderTargets(uint32_t width, uint32_t height)
	{
		s_RenderTarget->Resize(width, height);

		s_Width = width;
		s_Height = height;
	}
}
