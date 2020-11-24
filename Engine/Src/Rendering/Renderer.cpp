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
	}

	bool Renderer::Initialize()
	{
		// ��¿ �� ����. Engine���κ��� WindowData�� ���ͼ� �ʱ�ȭ����.
		const auto& wndData = m_Manager->GetEngine()->GetWindowData();

		// IDXGI settings

		// Create Device & SwapChain
		{
			m_RenderDevice = std::make_shared<RenderDevice>(wndData.hWnd, wndData.width, wndData.height, wndData.windowed);
			
			if (!m_RenderDevice->IsInitialized())
				return false;
		}

		// CreateViewport
		{
			m_Viewport.TopLeftX = 0.0f;
			m_Viewport.TopLeftY = 0.0f;
			m_Viewport.Width	= static_cast<float>(wndData.width);
			m_Viewport.Height	= static_cast<float>(wndData.height);
			m_Viewport.MinDepth = 0.0f;	// �ϵ� �ڵ�
			m_Viewport.MaxDepth = 1.0f;	// �ϵ� �ڵ�
		}

		// Create DepthStechilStates
		{
			m_DepthStencilStateEnabled	= std::make_shared<DepthStencilState>(m_RenderDevice, TRUE, D3D11_COMPARISON_LESS);
			m_DepthStencilStateDisabled = std::make_shared<DepthStencilState>(m_RenderDevice, FALSE, D3D11_COMPARISON_LESS);

			if (!m_DepthStencilStateEnabled->IsInitialized() || !m_DepthStencilStateDisabled->IsInitialized())
				return false;
		}

		// Create RasterizerStates
		{
			m_RasterizerStateCullNoneSolid		= std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_NONE, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_RasterizerStateCullBackSolid		= std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_BACK, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_RasterizerStateCullFrontSolid		= std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_FRONT, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_RasterizerStateCullNoneWireFrame	= std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			m_RasterizerStateCullBackWireFrame	= std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			m_RasterizerStateCullFrontWireFrame = std::make_shared<RasterizerState>(m_RenderDevice, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			
			if (!m_RasterizerStateCullNoneSolid->IsInitialized() || !m_RasterizerStateCullBackSolid->IsInitialized() || !m_RasterizerStateCullFrontSolid->IsInitialized()
				|| !m_RasterizerStateCullNoneWireFrame->IsInitialized() || !m_RasterizerStateCullBackWireFrame->IsInitialized() || !m_RasterizerStateCullFrontWireFrame->IsInitialized())
				return false;
		}

		// Create Blend States
		{
			m_BlendStateEnable		= std::make_shared<BlendState>(m_RenderDevice, TRUE);
			m_BlendStateDisable		= std::make_shared<BlendState>(m_RenderDevice);
			m_BlendStateColorAdd	= std::make_shared<BlendState>(m_RenderDevice, TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

			if (!m_BlendStateEnable->IsInitialized() || !m_BlendStateDisable->IsInitialized() || !m_BlendStateColorAdd->IsInitialized())
				return false;
		}

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

	// ������κ��� ũ�Ⱑ ����Ǿ��ٴ� �޽���
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (nullptr == m_RenderDevice)
			return;

		m_RenderDevice->ResizeBuffer(evnt->GetWidth(), evnt->GetHeight());
	}
}
