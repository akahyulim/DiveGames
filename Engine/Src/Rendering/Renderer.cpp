#include "DivePch.h"
#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "D3D11/RenderDevice.h"
#include "D3D11/SwapChain.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{
		EventSystem::GetInstance().Subscribe(this, &Renderer::OnResize);
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		// 어쩔 수 없다. Engine으로부터 WindowData를 얻어와서 초기화하자.
		const auto& wndData = m_Manager->GetEngine()->GetWindowData();

		// initialize device
		m_RenderDevice = std::make_unique<RenderDevice>();
		

		// intialize swap chain
		// hwnd, resolution, vsync, windowed, target frame, render device
		m_SwapChain = std::make_unique<SwapChain>();

		// initialize resource

		// set viewport
		
		return true;
	}
	
	void Renderer::Update()
	{
		if (!m_RenderDevice || !m_RenderDevice->GetImmediateContext())
		{
			//CORE_WARN("Renderer::Update>> RenderDevice가 준비되지 않았습니다.");
			return;
		}

		// camera 존재 유무

		// set constant buffer data

		// rendering path(pass 묶음)
	}

	void Renderer::Present()
	{
		if (!m_RenderDevice)
			return;
	}

	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (!m_SwapChain || !evnt)
			return;

		m_SwapChain->ResizeResolution(evnt->GetHeight(), evnt->GetWidth());
	}
}
