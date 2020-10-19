#include "DivePch.h"
#include "Renderer.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "D3D11/RenderDevice.h"

namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{

		// initialize device
		// window ������ �ʿ��ϴ�.
		// ���� interface�� ������ intialize �޼��庸�� �����ڿ��� �����ϴ� �� ����.
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
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
}
