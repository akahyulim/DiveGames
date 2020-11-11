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

		m_RenderDevice = std::make_unique<RenderDevice>();
		m_SwapChain = std::make_unique<SwapChain>();
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		const auto& windowData = m_Manager->GetEngine()->GetWindowData();

		// initialize device
		// intialize swap chain

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

		m_RenderDevice->Present();
	}

	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (!m_SwapChain || !evnt)
			return;

		m_SwapChain->ResizeResolution(evnt->GetHeight(), evnt->GetWidth());
	}
}
