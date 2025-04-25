#include "stdafx.h"
#include "Renderer.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "graphics/Graphics.h"
#include "graphics/RenderTexture.h"
#include "world/Components.h"

namespace Dive
{
	UINT32 Renderer::s_Width = 1280;
	UINT32 Renderer::s_Height = 760;
	std::unique_ptr<RenderTexture> Renderer::s_RenderTarget;

	void Renderer::Initialize()
	{
		Shutdown();

		// 각종 리소스 생성(렌더타겟, states)
		// 코파일럿은 ResourceManager를 별도로 만들라고 한다.

		s_RenderTarget = std::make_unique<RenderTexture>(s_Width, s_Height);
	}
	
	void Renderer::Shutdown()
	{

		// 생성한 리소스 릴리즈
	}

	void Renderer::RenderScene(const entt::registry& registry)
	{
		DV_FIRE_EVENT(eEventType::PreRender);
		
		{

		}

		DV_FIRE_EVENT(eEventType::PostRender);
	}

	void Renderer::Present()
	{
		Graphics::Present();
	}

	void Renderer::ResizeRenderTargets(UINT32 width, UINT32 height)
	{
		s_RenderTarget->Resize(width, height);

		s_Width = width;
		s_Height = height;
	}
}
