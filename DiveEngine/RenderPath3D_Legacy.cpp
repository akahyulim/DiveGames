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
			// Wicked에서 RenderPass는 RenerTargets이며 ResizeBuffers()에서 Texture들 생성 후 구성된다.
			// 이후 이 곳에서 GraphicesDevice의 Begin() / End()를 통해 RenderTarget으로 설정된다.
			// 이를 통해 복잡한 코드를 해결한 듯 하다.
			// 여기에서 다시 GraphicsDevice를 사용 하느냐, ImmediateContext를 직접 사용하느냐로 구현이 갈린다.
			// 즉, GraphicsDevice의 역할에 대해 정리를 마쳐야 한다.

			// Pass Begin: Set & Clear RenderTargets, DepthStencilView
			{
				ID3D11RenderTargetView* pGBuffer[] = {
					m_pGBuffer[eGBuffer::RT0]->GetColorRenderTargetView(),
					m_pGBuffer[eGBuffer::RT1]->GetColorRenderTargetView(),
					m_pGBuffer[eGBuffer::RT2]->GetColorRenderTargetView() };

				// RTV, DSV 초기화
				pImmediateContext->OMSetRenderTargets(3, pGBuffer, m_pDepthStencilBuffer->GetDepthStencilView());
			}

			// Bind Viewport
			// 이건 일단 Draw에 넣자.
		
			// Draw
			{

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

		// Camera의 크기를 갱신해주는 과정이다.
		// 이를 통해 Viewport 또한 계산되므로 꼭 필요하다.
		// 다만 그 위치가 애매하다.
		// Wicked의 경우 현재와 같이 RenderTarget 설정 후 설정하는데
		// 다중 카메라를 지원토록 하려면 Pass 함수 안에서 하는게 맞다.
		unsigned int width = pGraphicsDevice->GetResolutionWidth();
		unsigned int height = pGraphicsDevice->GetResolutionHeight();
		Renderer::GetInstance().SetViewport(width, height);

		Renderer::GetInstance().DrawScene();
	}
}