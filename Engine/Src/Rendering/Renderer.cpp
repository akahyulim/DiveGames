#include "DivePch.h"
#include "Renderer.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "D3D11/RenderDevice.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{
		// 매개변수 변경이 가능한 이 곳에서 윈도우 정보를 받는게 가장 적합해보인다.
		// 하지만 그렇게 할 경우 관련 멤버 변수를 따로 만들지 않는 이상 이 곳에서 RenderDevice를 생성해야 한다.
		// 물론 RenderDevice의 생성자의 매개변수로 넘겨주는 방법이 있지만
		// RenderDevice도 역시 widow handle과 instance가 생성시외엔 필요가 없다.
		
		// 다른 방법으로는 전역 혹은 system manager 등을 통해 얻어오는 경우를 생각해 볼 수 있다.

		// 필요한 정보로는
		// handle, instance, width, height, windowed, vsync 등이 있다.
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		// initialize device
		// window 정보가 필요하다.
	
		// initialize resource

		// set viewport
		
		return true;
	}
	
	void Renderer::Update()
	{
		if (!m_render_device || !m_render_device->GetImmediateContext())
		{
			CORE_WARN("Renderer::Update>> RenderDevice가 준비되지 않았습니다.");
			return;
		}

		// camera 존재 유무

		// set constant buffer data

		// rendering path(pass 묶음)
	}

	void Renderer::Present()
	{
		if (!m_render_device)
			return;

		m_render_device->Present();
	}

	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (!m_render_device)
			return;

		m_render_device->ResizeResolution(evnt->GetHeight(), evnt->GetWidth());
	}
}
