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
		// window 정보가 필요하다.
		// 따라서 interface가 정해진 intialize 메서드보다 생성자에서 생성하는 게 낫다.
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
			CORE_WARN("Renderer::Update>> RenderDevice가 준비되지 않았습니다.");
			return;
		}

		// camera 존재 유무

		// set constant buffer data

		// rendering path(pass 묶음)
	}
}
