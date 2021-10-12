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
	// ��, �� ������ ���� �׸��°� �ƴϴ�.
	void RenderPath3D_Legacy::ResizeBuffers()
	{
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		// resolution
		// �̷��� �ȵ� �� ������... �Ϲ������δ� �´�.
		// ������ Editor������ ũ�⸦ �ٸ��� �����ؾ� �Ѵ�.
		// �׷��ٸ� �̺�Ʈ�� ���� ũ�⸦ ���޹����� ���?
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
		// ������ �Ű������� �޾� �����ϰ�, �� ������ ���� ������ Ȯ���ϴ� ���� ��������...?
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

				// �̰� �´� �� ����. RT���� ũ�Ⱑ �޶��� �� �ֱ� �����̴�.
				// �ٸ� å�� ���������� �� �� �� Backbuffer�� ũ��� �����ߴ�. �׳� �ܼ��ϰ� ���� �� ���⵵ �ϰ�...
				D3D11_VIEWPORT vp;
				vp.Width = width;
				vp.Height = height;
				vp.TopLeftX = 0.0f;
				vp.TopLeftY = 0.0f;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				pImmediateContext->RSSetViewports(1, &vp);

				// ���ó� ī�޶� �ɸ���.
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
				// �� �����ص� �̰� �´� �� ����.
				ID3D11RenderTargetView* pGBuffer[] = { nullptr, nullptr, nullptr };
				pImmediateContext->OMSetRenderTargets(3, pGBuffer, nullptr);
			}
		}

		// Pass - Light
		{
		}
	}

	// Backbuffer�� RenderTarget���� �ϴ� Begine / End�� �ѷ� �׿� �ִ�.
	void RenderPath3D_Legacy::Compose() const
	{
		// �׽�Ʈ
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

		// ī�޶󿡵� ũ�⸦ �����ؾ� �ϴ� �� ����.
		// ���ʿ� �� �Լ��� viewport���� �����ϴ� �ǵ����� �� ����.
		Renderer::GetInstance().SetViewport(width, height);

		// ���� Renderable�� ȭ�鿡�� ������ �ʴ´�.
		Renderer::GetInstance().DrawScene();
	}
}