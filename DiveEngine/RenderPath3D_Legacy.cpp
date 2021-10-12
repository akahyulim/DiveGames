#include "RenderPath3D_Legacy.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
#include "Timer.h"
#include "Event.h"
#include "Log.h"

namespace dive
{
	// 생성자에서 처리하기 조금 애매하다.
	RenderPath3D_Legacy::RenderPath3D_Legacy()
	{
		ResizeBuffers();
		EVENT_SUBSCRIBE(eEventType::ChangedResolution, EVENT_HANDLE(ResizeBuffers));
	}
	
	RenderPath3D_Legacy::~RenderPath3D_Legacy()
	{
	}

	// Wicked에선 Resolution Change 이벤트에 반응하는듯 하다.
	// 즉, 매 프레임 새로 그리는건 아니다.
	void RenderPath3D_Legacy::ResizeBuffers()
	{
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		// resolution
		// 이러면 안될 것 같은데... 일반적으로는 맞다.
		// 하지만 Editor에서는 크기를 다르게 설정해야 한다.
		// 그렇다면 이벤트를 통해 크기를 전달받으면 어떨까?
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
		if (!m_pScene)
			return;

		// 이걸 매 프레임마다 가져 오는 것도 오버헤드일텐데...
		// 생성시 매개변수로 받아 저장하고, 매 프레임 존재 유무를 확인하는 편이 나으려나...?
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		auto pImmediateContext = pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);

		// Pass - GBuffer
		{
			// Pass Begin: Set & Clear RenderTargets, DepthStencilView
			{
				ID3D11RenderTargetView* pGBuffer[] = {
					m_pGBuffer[eGBuffer::RT0]->GetColorRenderTargetView(),
					m_pGBuffer[eGBuffer::RT1]->GetColorRenderTargetView(),
					m_pGBuffer[eGBuffer::RT2]->GetColorRenderTargetView() };

				float width = static_cast<float>(m_pGBuffer[eGBuffer::RT0]->GetWidth());
				float height = static_cast<float>(m_pGBuffer[eGBuffer::RT0]->GetHeight());

				// 이게 맞는 것 같다. RT마다 크기가 달라질 수 있기 때문이다.
				// 다만 책의 예제에서는 딱 한 번 Backbuffer의 크기로 설정했다. 그냥 단순하게 만든 것 같기도 하고...
				D3D11_VIEWPORT vp;
				vp.Width = width;
				vp.Height = height;
				vp.TopLeftX = 0.0f;
				vp.TopLeftY = 0.0f;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				pImmediateContext->RSSetViewports(1, &vp);

				// 역시나 카메라가 걸린다.
				Renderer::GetInstance().SetViewport(width, height);

				float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
				pImmediateContext->ClearRenderTargetView(pGBuffer[0], clearColor);
				pImmediateContext->ClearRenderTargetView(pGBuffer[1], clearColor);
				pImmediateContext->ClearRenderTargetView(pGBuffer[2], clearColor);
				pImmediateContext->ClearDepthStencilView(m_pDepthStencilBuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				pImmediateContext->OMSetRenderTargets(3, pGBuffer, m_pDepthStencilBuffer->GetDepthStencilView());
			}

			// Draw
			{
				Renderer::GetInstance().DrawGBuffer();
			}

			// Pass End: Disconect RenderTargets
			{
				// 좀 생소해도 이게 맞는 것 같다.
				ID3D11RenderTargetView* pGBuffer[] = { nullptr, nullptr, nullptr };
				pImmediateContext->OMSetRenderTargets(3, pGBuffer, nullptr);
			}
		}

		// Pass - Light
		{
		}
	}

	// Backbuffer를 RenderTarget으로 하는 Begine / End에 둘러 쌓여 있다.
	void RenderPath3D_Legacy::Compose() const
	{
		// 테스트
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);
		auto width = (float)pGraphicsDevice->GetResolutionWidth();
		auto height = (float)pGraphicsDevice->GetResolutionHeight();

		D3D11_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		pGraphicsDevice->GetImmediateContext()->RSSetViewports(1, &vp);

		// 카메라에도 크기를 전달해야 하는 것 같다.
		// 애초에 이 함수가 viewport까지 설정하는 의도였던 것 같다.
		Renderer::GetInstance().SetViewport(width, height);

		// 현재 Renderable이 화면에서 보이지 않는다.
		Renderer::GetInstance().DrawScene();
	}
}