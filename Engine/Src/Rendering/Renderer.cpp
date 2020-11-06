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
		// �Ű����� ������ ������ �� ������ ������ ������ �޴°� ���� �����غ��δ�.
		// ������ �׷��� �� ��� ���� ��� ������ ���� ������ �ʴ� �̻� �� ������ RenderDevice�� �����ؾ� �Ѵ�.
		// ���� RenderDevice�� �������� �Ű������� �Ѱ��ִ� ����� ������
		// RenderDevice�� ���� widow handle�� instance�� �����ÿܿ� �ʿ䰡 ����.
		
		// �ٸ� ������δ� ���� Ȥ�� system manager ���� ���� ������ ��츦 ������ �� �� �ִ�.

		// �ʿ��� �����δ�
		// handle, instance, width, height, windowed, vsync ���� �ִ�.
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		// initialize device
		// window ������ �ʿ��ϴ�.
	
		// initialize resource

		// set viewport
		
		return true;
	}
	
	void Renderer::Update()
	{
		if (!m_render_device || !m_render_device->GetImmediateContext())
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
