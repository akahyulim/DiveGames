#include "DivePch.h"
#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "D3D11/RenderDevice.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{
		m_RenderDevice = std::make_unique<RenderDevice>();
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		const auto& windowData = m_Manager->GetEngine()->GetWindowData();

		/*
		// initialize device
		if (!m_RenderDevice->Initialize(
			windowData.hWnd,
			windowData.ResolutionWidth,
			windowData.ResolutionHeight,
			windowData.bVSync,
			windowData.bWindowed))
		{
			CORE_ERROR("Renderer::Initialize>> Renderer ��ü �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}
	*/

		// initialize resource

		// set viewport
		
		return true;
	}
	
	void Renderer::Update()
	{
		if (!m_RenderDevice || !m_RenderDevice->GetImmediateContext())
		{
			CORE_WARN("Renderer::Update>> RenderDevice�� �غ���� �ʾҽ��ϴ�.");
			return;
		}

		// camera ���� ����

		// set constant buffer data

		// rendering path(pass ����)
	}

	void Renderer::Present()
	{
		if (!m_RenderDevice)
			return;

		m_RenderDevice->Present();
	}

	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (!m_RenderDevice)
			return;

		m_RenderDevice->ResizeResolution(evnt->GetHeight(), evnt->GetWidth());
	}
}
