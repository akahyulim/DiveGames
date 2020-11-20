#include "DivePch.h"
#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "Events/WindowEvent.h"
#include "D3D11/RenderDevice.h"
#include "D3D11/RenderDeviceEnums.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/BlendState.h"
#include "D3D11/Texture2D.h"


namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{
		EventSystem::GetInstance().Subscribe(this, &Renderer::OnResize);
	}

	Renderer::~Renderer()
	{
		// RastarizerStates
		{
			SAFE_DELETE(m_RasterizerStateCullFrontWireFrame);
			SAFE_DELETE(m_RasterizerStateCullBackWireFrame);
			SAFE_DELETE(m_RasterizerStateCullNoneWireFrame);
			SAFE_DELETE(m_RasterizerStateCullFrontSolid);
			SAFE_DELETE(m_RasterizerStateCullBackSolid);
			SAFE_DELETE(m_RasterizerStateCullNoneSolid);
		}

		// DepthStencilStates
		{
			SAFE_DELETE(m_DepthStencilStateEnabled);
			SAFE_DELETE(m_DepthStencilStateDisabled);
		}

		// Device & SwapChain
		{
			SAFE_DELETE(m_RenderDevice);
		}
	}

	bool Renderer::Initialize()
	{
		// 어쩔 수 없다. Engine으로부터 WindowData를 얻어와서 초기화하자.
		const auto& wndData = m_Manager->GetEngine()->GetWindowData();

		// IDXGI settings

		// Create Device & SwapChain
		m_RenderDevice = new RenderDevice(wndData.hWnd, wndData.width, wndData.height, wndData.windowed);
		if (!m_RenderDevice->IsInitialized())
			return false;

		// Create DepthStechilStates
		m_DepthStencilStateEnabled	= new DepthStencilState(m_RenderDevice, TRUE, D3D11_COMPARISON_LESS);
		m_DepthStencilStateDisabled	= new DepthStencilState(m_RenderDevice, FALSE, D3D11_COMPARISON_LESS);
		if (!m_DepthStencilStateEnabled->IsInitialized() || !m_DepthStencilStateDisabled->IsInitialized())
			return false;

		// Create RasterizerStates
		m_RasterizerStateCullNoneSolid		= new RasterizerState(m_RenderDevice, D3D11_CULL_NONE, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_RasterizerStateCullBackSolid		= new RasterizerState(m_RenderDevice, D3D11_CULL_BACK, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_RasterizerStateCullFrontSolid		= new RasterizerState(m_RenderDevice, D3D11_CULL_FRONT, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_RasterizerStateCullNoneWireFrame	= new RasterizerState(m_RenderDevice, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_RasterizerStateCullBackWireFrame	= new RasterizerState(m_RenderDevice, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_RasterizerStateCullFrontWireFrame = new RasterizerState(m_RenderDevice, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		if (!m_RasterizerStateCullNoneSolid->IsInitialized() || !m_RasterizerStateCullBackSolid->IsInitialized() || !m_RasterizerStateCullFrontSolid->IsInitialized()
			|| !m_RasterizerStateCullNoneWireFrame->IsInitialized() || !m_RasterizerStateCullBackWireFrame->IsInitialized() || !m_RasterizerStateCullFrontWireFrame->IsInitialized())
			return false;

		// Create Blend States
		m_BlendStateEnable		= new BlendState(m_RenderDevice, TRUE);
		m_BlendStateDisable		= new BlendState(m_RenderDevice);
		m_BlendStateColorAdd	= new BlendState(m_RenderDevice, TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
		if (!m_BlendStateEnable->IsInitialized() || !m_BlendStateDisable->IsInitialized() || !m_BlendStateColorAdd->IsInitialized())
			return false;

		return true;
	}
	
	void Renderer::Update()
	{
	}

	void Renderer::Present()
	{
		if (!m_RenderDevice->IsInitialized())
			return;

		m_RenderDevice->Present();
	}

	// 윈도우로부터 크기가 변경되었다는 메시지
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (nullptr == m_RenderDevice)
			return;

		m_RenderDevice->ResizeBuffer(evnt->GetWidth(), evnt->GetHeight());
	}
}
