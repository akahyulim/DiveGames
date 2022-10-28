#include "divepch.h"
#include "Renderer.h"
#include "RendererEvents.h"
#include "RenderPath.h"
#include "Viewport.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "Engine/EngineEvents.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "IO/Log.h"


namespace Dive
{
	Renderer::Renderer(Context* pContext)
		: Object(pContext),
		m_pGraphics(nullptr),
		m_bInitialized(false)
	{
		// ScreenMode �޽��� ����

		// ��ũ�� ��尡 �������� �ʾҴٸ� ��ŵ�Ѵܴ�..
		// ��� ��ŵ�ϴ��� ���� Ȯ�� �Ұ�
		// �ٵ� ���ο��� Graphics�� �ʱ�ȭ�� Ȯ���Ѵ�.
		// ���� �� �������� �ʱ�ȭ�� ������ �� ����.
		initialize();
	}

	Renderer::~Renderer()
	{
		DV_LOG_ENGINE_DEBUG("Renderer �Ҹ��� ȣ��");

		for (int i = 0; i < static_cast<int>(m_Viewports.size()); ++i)
			DV_DELETE(m_Viewports[i]);
		m_Viewports.clear();
	}
	
	// EventHandler�� ȣ���Ѵ�.
	void Renderer::Update(float delta)
	{
		m_Views.clear();

		if (!m_pGraphics || !m_pGraphics->IsInitialized() || m_pGraphics->IsDeviceLost())
			return;

		for (auto i = static_cast<int>(m_Viewports.size()) - 1; i >= 0; --i)
			queueViewport(nullptr, m_Viewports[i]);
		
		unsigned int numMainViewports = static_cast<unsigned int>(m_QueuedViewports.size());
		for (unsigned int i = 0; i < numMainViewports; ++i)
			updateQueuedViewport(i);

		FIRE_EVENT(RenderTargetUpdate());

		for (unsigned int i = numMainViewports; i < static_cast<unsigned int>(m_QueuedViewports.size()); ++i)
			updateQueuedViewport(i);

		m_QueuedViewports.clear();
	}
	
	void Renderer::Render()
	{
		// �ӽ�: ���� initialize() ȣ���� ���� ����.
		// ���� �̸� �����ϰ� OnScreenMode�� ó��.
		if (!m_bInitialized)
		{
			initialize();
		}

		// assert�� graphics �ʱ�ȭ �� lost Ȯ��
		// �ڽ��� initialize�� Ȯ���ؾ� �� �� ������...
		DV_ASSERT(m_pGraphics || m_pGraphics->IsInitialized() || !m_pGraphics->IsDeviceLost());

		// �������� Views' Render.
		if (!m_Views.empty())
		{
			DV_LOG_ENGINE_DEBUG("numViews: {:d}", (int)m_Views.size());

			for (int i = static_cast<int>(m_Views.size() - 1); i >= 0; --i)
			{
				if (!m_Views[i])
					continue;

				m_Views[i]->Render();
			}
		}
	}

	void Renderer::OnRenderUpdate(const Event& e)
	{
		auto& evnt = dynamic_cast<const RenderUpdateEvent&>(e);

		Update(evnt.GetDeltaTime());
	}

	// �̰� ȣ��Ƿ��� OnScreenModeChanged�� OnWindowResized�� ȣ��Ǿ�� �Ѵ�.
	// orho�� SetScreenMode�� �������� OnScreenModeChagned�� ȣ���Ѵ�.
	// OnScreenModeChanged�� ������ �α׷� ����ϰ�, �����͸� ���ڷ� �̺�Ʈ�� �����Ѵ�.
	// private�� ������ ���� �̶�(SetScreenMode)�� ȣ��Ǵ� �� �ϴ�.
	void Renderer::OnScreenMode(const Event& e)
	{
		if (!m_bInitialized)
			initialize();

		// �̹� �����Ǿ��ٸ� m_bViewReset = true;
	}

	Viewport* Renderer::GetViewport(unsigned int index)
	{
		return m_Viewports.size() > index ? m_Viewports[index] : nullptr;
	}

	void Renderer::SetViewport(unsigned int index, Viewport* pViewport)
	{
		if (m_Viewports.size() >= index)
			m_Viewports.insert(m_Viewports.begin() + index, pViewport);
		else
			m_Viewports.emplace_back(pViewport);
	}

	// �̿ܿ��� �� ������ �� ���޹޴´�.
	Texture* Renderer::GetScreenBuffer(int width, int height, DXGI_FORMAT format, bool bCubemap)
	{
		// format���� depthstencil ���θ� �Ǻ�

		auto searchKey = static_cast<unsigned long long>(format) << 32u | width << 24u | height << 16u;
		
		if (m_ScreenBuffers.find(searchKey) == m_ScreenBuffers.end())
			m_ScreenBufferIndices[searchKey] = 0;

		auto index = m_ScreenBufferIndices[searchKey];

		if (index >= static_cast<unsigned int>(m_ScreenBuffers[searchKey].size()))
		{
			Texture* pNewBuffer = nullptr;

			if (!bCubemap)
			{
				Texture2D* pNewTex2D = new Texture2D(m_pContext);
				// depthstencil ���η� usage ����.
				pNewTex2D->SetSize(width, height, format);

				pNewBuffer = pNewTex2D;
			}
			else
			{
				// cubemap ����
			}

			// ��Ÿ ����
			m_ScreenBuffers[searchKey].emplace_back(pNewBuffer);

			return pNewBuffer;
		}
		else
		{
			auto* pBuffer = m_ScreenBuffers[searchKey][index];
			// reset user time?
			return pBuffer;
		}

		return nullptr;
	}

	void Renderer::QueueViewportByRenderTarget(Texture* pRenderTarget)
	{
		if (!pRenderTarget)
			return;

		auto numViewports = pRenderTarget->GetNumViewports();
		for (unsigned int i = 0; i < numViewports; ++i)
			queueViewport(pRenderTarget, pRenderTarget->GetViewport(i));
	}

	void Renderer::initialize()
	{
		auto pGraphics = GetSubsystem<Graphics>();
		// cache manager

		// cache manager �� Ȯ�� �ʿ�
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		// �Ϲ������ͷ� �޾� weak_ptr�� ���� ���Ѵ�...
		// ������ �Ϲ� �ּҰ����δ� weak_ptr�� ������ �� ���ٰ� �Ѵ�...
		m_pGraphics = pGraphics;

		// cache manager�� ���� load
		// default light ramp
		// default light spot

		// default material ����

		// ��ü�� ������ �� cache manager�κ��� load
		// default render path

		// create geometries
		// create instancing buffer

		// reset shadow map
		// reset buffers

		m_bInitialized = true;

		SUBSCRIBE_EVENT(eEventType::RenderUpdate, EVENT_HANDLER_PARAM(OnRenderUpdate));

		DV_LOG_ENGINE_INFO("Renderer �ʱ�ȭ ����");
	}

	void Renderer::queueViewport(Texture* pRenderTarget, Viewport* pViewport)
	{
		if (!pViewport)
			return;

		m_QueuedViewports.emplace_back(std::make_pair(pRenderTarget, pViewport));
	}

	void Renderer::updateQueuedViewport(unsigned int index)
	{
		auto* pRenderTarget = m_QueuedViewports[index].first;
		auto* pViewport = m_QueuedViewports[index].second;

		if (!pViewport->GetView())
			pViewport->AllocateView();

		auto* pView = pViewport->GetView();
		DV_ASSERT(pView);

		if (!pView->Define(pRenderTarget, pViewport))
			return;

		m_Views.emplace_back(pView);

		// ������ FrameInfo�� �����Ѵ�.
		// �׸��� �̴� ��� �����̹Ƿ�, ���⿡�� ȣ���� �����ϴ�.
		pView->Update(1.0f);
	}
}