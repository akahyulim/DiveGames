#include "RenderPath3D_Legacy.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
#include "Timer.h"
#include "Log.h"

namespace dive
{
	// 주기를 아직 모르겠다.
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

		CORE_TRACE("RenderPath3D_Legacy::PreUpdate() - Previous Camera 저장");

		// GameObject PreUpdate를 수행할 수 있다.
	}

	// 그리기전 준비를 한다.
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
	
	// Camera까지 생각하면 머리가 뽀개질 것 같다....
	// 결국 하나 하나 다 구성해본 후 모듈화해야 할 것 같다.
	// 그리고 Sandbox에서 확인하면 구현해야 한다.
	void RenderPath3D_Legacy::Render() const
	{
		// 이걸 매 프레임마다 가져 오는 것도 오버헤드일텐데...
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		auto pImmediateContext = pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);


		if (!m_pScene)
		{
			// back buffer라도 그려줘야지
			// 근데 역시 흰 바탕이라 차이가 없다.
			pGraphicsDevice->PresentBegin();
			pGraphicsDevice->PresentEnd();

			// 그리고 이 리턴이 마음에 들지 않는데...
			return;
		}

		
		// GBuffer Pass
		// : Opaque Object를 각각의 GBuffer에 그린다.
		{
			//Pass Begin
			{
				ID3D11RenderTargetView* pRTV[3] =
				{ m_pGBuffer[eGBuffer::RT0]->GetColorRenderTargetView(), m_pGBuffer[eGBuffer::RT1]->GetColorRenderTargetView(), m_pGBuffer[eGBuffer::RT2]->GetColorRenderTargetView() };
				float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				pImmediateContext->ClearRenderTargetView(pRTV[0], clearColor);
				pImmediateContext->ClearRenderTargetView(pRTV[1], clearColor);
				pImmediateContext->ClearRenderTargetView(pRTV[2], clearColor);

				pImmediateContext->ClearDepthStencilView(m_pDepthStencilBuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				pImmediateContext->OMSetRenderTargets(3, pRTV, m_pDepthStencilBuffer->GetDepthStencilView());
			}
			
			// Bind
			// : Renderer 내부에서 구현하면 간단하지만, 이 곳에서 구현하면 Get으로 얻어온 후 다시 Bind해야 한다...
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
			// : opaque object만 대상이다.
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
				}
			}

			// End Pass
			{
				ID3D11RenderTargetView* pRTV[3] = { nullptr, nullptr, nullptr };
				pGraphicsDevice->GetImmediateContext()->OMSetRenderTargets(3, pRTV, nullptr);
			}
		}

		// Lighting Pass
		{

		}

		// Post Processing Passes
		{
		}
	}

	void RenderPath3D_Legacy::Compose() const
	{
		// 기본적으로는 Backbuffer에 그린다.
	}
}