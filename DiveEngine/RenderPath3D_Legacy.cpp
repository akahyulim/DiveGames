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
		// ������ �Ű������� �޾� �����ϰ�, �� ������ ���� ������ Ȯ���ϴ� ���� ��������...?
		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		DV_ASSERT(pGraphicsDevice);

		auto pImmediateContext = pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);
		
		// Pass - GBuffer
		{
			// Wicked���� RenderPass�� RenerTargets�̸� ResizeBuffers()���� Texture�� ���� �� �����ȴ�.
			// ���� �� ������ GraphicesDevice�� Begin() / End()�� ���� RenderTarget���� �����ȴ�.
			// �̸� ���� ������ �ڵ带 �ذ��� �� �ϴ�.
			// ���⿡�� �ٽ� GraphicsDevice�� ��� �ϴ���, ImmediateContext�� ���� ����ϴ��ķ� ������ ������.
			// ��, GraphicsDevice�� ���ҿ� ���� ������ ���ľ� �Ѵ�.

			// Pass Begin: Set & Clear RenderTargets, DepthStencilView
			// Bind Viewport
			// Draw
			// Pass End: Disconect RenderTargets
		}

		// Pass - Light
		{

		}
	}

	void RenderPath3D_Legacy::Compose() const
	{
		// �⺻�����δ� Backbuffer�� �׸���.
	}
}