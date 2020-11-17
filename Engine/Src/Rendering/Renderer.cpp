#include "DivePch.h"
#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "Events/WindowEvent.h"
#include "D3D11/RenderDevice.h"

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

		// IDXGI settings

		// create Device & SwapChain
		m_RenderDevice = new RenderDevice(wndData.hWnd, wndData.width, wndData.height, wndData.windowed);
		if (!m_RenderDevice->IsInitialized())
			return false;

		return true;
	}
	
	void Renderer::Update()
	{
	}

	void Renderer::Present()
	{
	}

	// 윈도우로부터 크기가 변경되었다는 메시지
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
	}
}
