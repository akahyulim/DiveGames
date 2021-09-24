#include "RenderPath3D_Legacy.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
#include "Timer.h"
#include "Event.h"
#include "Log.h"

namespace dive
{
	// �����ڿ��� ó���ϱ� ���� �ָ��ϴ�.
	RenderPath3D_Legacy::RenderPath3D_Legacy()
	{
		ResizeBuffers();
		EVENT_SUBSCRIBE(eEventType::ChangedResolution, EVENT_HANDLE(ResizeBuffers));
	}
	
	RenderPath3D_Legacy::~RenderPath3D_Legacy()
	{
	}

	// Wicked���� Resolution Change �̺�Ʈ�� �����ϴµ� �ϴ�.
	void RenderPath3D_Legacy::ResizeBuffers()
	{
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		// resolution
		unsigned int width = pGraphicsDevice->GetResolutionWidth();
		unsigned int height = pGraphicsDevice->GetResolutionHeight();

		// craete Render Targets
		{
			// GBuffers
			{
				DV_DELETE(m_pGBuffer[eGBuffer::RT0]);
				DV_DELETE(m_pGBuffer[eGBuffer::RT1]);
				DV_DELETE(m_pGBuffer[eGBuffer::RT2]);

				m_pGBuffer[eGBuffer::RT0] = new dvRenderTexture(width, height, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
				if (m_pGBuffer[eGBuffer::RT0])	m_pGBuffer[eGBuffer::RT0]->Create();

				m_pGBuffer[eGBuffer::RT1] = new dvRenderTexture(width, height, 0, DXGI_FORMAT_R11G11B10_FLOAT);
				if (m_pGBuffer[eGBuffer::RT1])	m_pGBuffer[eGBuffer::RT1]->Create();

				m_pGBuffer[eGBuffer::RT2] = new dvRenderTexture(width, height, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
				if (m_pGBuffer[eGBuffer::RT2])	m_pGBuffer[eGBuffer::RT2]->Create();
			}

			// DepthStencil Buffer
			{
				DV_DELETE(m_pDepthStencilBuffer);

				m_pDepthStencilBuffer = new dvRenderTexture(width, height, 32);
				if (m_pDepthStencilBuffer)	m_pDepthStencilBuffer->Create();
			}
		}

		// create RenderPass?
	}

	void RenderPath3D_Legacy::PreUpdate()
	{
		if (!m_pScene)	return;

		// GameObject PreUpdate�� ������ �� �ִ�.
	}

	// �׸����� �غ� �Ѵ�.
	void RenderPath3D_Legacy::Update(float deltaTime)
	{
		if (!m_pScene)	return;

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// common constant buffer
		{

		}
	}
	
	// Camera���� �����ϸ� �Ӹ��� �ǰ��� �� ����....
	// �ᱹ �ϳ� �ϳ� �� �����غ� �� ���ȭ�ؾ� �� �� ����.
	// �׸��� Sandbox���� Ȯ���ϸ� �����ؾ� �Ѵ�.
	void RenderPath3D_Legacy::Render() const
	{
		if (!m_pScene)
			return;

		// �̰� �� �����Ӹ��� ���� ���� �͵� ����������ٵ�...
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		auto pImmediateContext = pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);
		
		// GBuffer Pass
		// : Opaque Object�� ������ GBuffer�� �׸���.
		{
			//Pass Begin
			{
				ID3D11RenderTargetView* pRTV[3] = { 
					m_pGBuffer[eGBuffer::RT0]->GetColorRenderTargetView(), 
					m_pGBuffer[eGBuffer::RT1]->GetColorRenderTargetView(), 
					m_pGBuffer[eGBuffer::RT2]->GetColorRenderTargetView() };
				float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				pImmediateContext->ClearRenderTargetView(pRTV[0], clearColor);
				pImmediateContext->ClearRenderTargetView(pRTV[1], clearColor);
				pImmediateContext->ClearRenderTargetView(pRTV[2], clearColor);
				pImmediateContext->ClearDepthStencilView(m_pDepthStencilBuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				pImmediateContext->OMSetRenderTargets(3, pRTV, m_pDepthStencilBuffer->GetDepthStencilView());
			}
			
			// Bind
			// : Renderer ���ο��� �����ϸ� ����������, �� ������ �����ϸ� Get���� ���� �� �ٽ� Bind�ؾ� �Ѵ�...
			{
				// gbuffer constant buffer
				// viewport
				// depth stencil state
				// rasterizer state
				// sampler state
				// primitive topology
				// vertex shader
				// input layout
				// pixel shader
			}

			// Draw
			// : opaque object�� ����̴�.
			{
				// bind
				{
					// object constant buffer
					// vertex buffer
					// index buffer
					// material resource
				}

				// draw
				{
					// �⺻������ ����� back buffer�� float4�� �Ѱ����Ŵ�.
					// ���� �ٸ��� �����ϰ� �ʹٸ� �߰� rtv�� �ʿ��ϴ�.
				}
			}

			// End Pass
			{
				ID3D11RenderTargetView* pRTV[3] = { nullptr, nullptr, nullptr };
				pImmediateContext->OMSetRenderTargets(3, pRTV, nullptr);
			}
		}

		// Lighting Pass
		{
			// GBuffer�� SRV�� Ȱ��
		}

		// Post Processing Passes
		{
		}
	}

	void RenderPath3D_Legacy::Compose() const
	{
		// �⺻�����δ� Backbuffer�� �׸���.
	}
}