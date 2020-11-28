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
	Renderer::Renderer(const std::shared_ptr<SystemManager>& manager)
		: System(manager)
	{
		EventSystem::GetInstance().Subscribe(this, &Renderer::OnResize);
	}

	Renderer::~Renderer()
	{
		CORE_TRACE("Call Renderer's Desctructor =====================");
	}

	bool Renderer::Initialize()
	{
		// ��¿ �� ����. Engine���κ��� WindowData�� ���ͼ� �ʱ�ȭ����.
		// �ᱹ �̰� ������.
		const auto& wndData = m_manager.lock()->GetEngine()->GetWindowData();
	
		// IDXGI settings

		// Create Device & SwapChain
		{
			m_renderDevice = std::make_shared<RenderDevice>(wndData.hWnd, wndData.width, wndData.height, wndData.windowed);
			
			if (!m_renderDevice->IsInitialized())
				return false;
		}

		// CreateViewport
		{
			m_viewport.TopLeftX = 0.0f;
			m_viewport.TopLeftY = 0.0f;
			m_viewport.Width	= static_cast<float>(wndData.width);
			m_viewport.Height	= static_cast<float>(wndData.height);
			m_viewport.MinDepth = 0.0f;	// �ϵ� �ڵ�
			m_viewport.MaxDepth = 1.0f;	// �ϵ� �ڵ�
		}

		// Create DepthStechilStates
		{
			m_depthStencilStateEnabled	= std::make_shared<DepthStencilState>(m_renderDevice, TRUE, D3D11_COMPARISON_LESS);
			m_depthStencilStateDisabled = std::make_shared<DepthStencilState>(m_renderDevice, FALSE, D3D11_COMPARISON_LESS);

			if (!m_depthStencilStateEnabled->IsInitialized() || !m_depthStencilStateDisabled->IsInitialized())
				return false;
		}

		// Create RasterizerStates
		{
			m_rasterizerStateCullNoneSolid		= std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_NONE, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_rasterizerStateCullBackSolid		= std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_BACK, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_rasterizerStateCullFrontSolid		= std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_FRONT, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
			m_rasterizerStateCullNoneWireFrame	= std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			m_rasterizerStateCullBackWireFrame	= std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			m_rasterizerStateCullFrontWireFrame = std::make_shared<RasterizerState>(m_renderDevice, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
			
			if (!m_rasterizerStateCullNoneSolid->IsInitialized() || !m_rasterizerStateCullBackSolid->IsInitialized() || !m_rasterizerStateCullFrontSolid->IsInitialized()
				|| !m_rasterizerStateCullNoneWireFrame->IsInitialized() || !m_rasterizerStateCullBackWireFrame->IsInitialized() || !m_rasterizerStateCullFrontWireFrame->IsInitialized())
				return false;
		}

		// Create Blend States
		{
			m_blendStateEnable		= std::make_shared<BlendState>(m_renderDevice, TRUE);
			m_blendStateDisable		= std::make_shared<BlendState>(m_renderDevice);
			m_blendStateColorAdd	= std::make_shared<BlendState>(m_renderDevice, TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

			if (!m_blendStateEnable->IsInitialized() || !m_blendStateDisable->IsInitialized() || !m_blendStateColorAdd->IsInitialized())
				return false;
		}

		return true;
	}
	
	void Renderer::Update()
	{
	}

	void Renderer::Present()
	{
		if (!m_renderDevice->IsInitialized())
			return;

		m_renderDevice->Present();
	}

	// ������κ��� ũ�Ⱑ ����Ǿ��ٴ� �޽���
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
		if (nullptr == m_renderDevice)
			return;

		m_renderDevice->ResizeBuffer(evnt->GetWidth(), evnt->GetHeight());
	}
}
