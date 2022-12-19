#include "divepch.h"
#include "Renderer.h"
#include "RendererEvents.h"
#include "RenderPath.h"
#include "Technique.h"
#include "Viewport.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "Engine/EngineEvents.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	// ���� Shader�� �����ϴ� ShaderVaritaion���� �迭�� ����� ���´�.
	// ���� ������ vs, ps�� Input Defines�� �� �ϴ�.


	Renderer::Renderer(Context* pContext)
		: Object(pContext),
		m_pGraphics(nullptr),
		m_pDefaultRenderPath(nullptr),
		m_pDefaultTechnique(nullptr),
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
		for (int32_t i = 0; i < static_cast<int32_t>(m_Viewports.size()); ++i)
			DV_DELETE(m_Viewports[i]);
		m_Viewports.clear();

		DV_LOG_ENGINE_TRACE("Renderer �Ҹ� �Ϸ�");
	}
	
	// EventHandler�� ȣ���Ѵ�.
	void Renderer::Update(float delta)
	{
		m_Views.clear();

		for (auto i = static_cast<int32_t>(m_Viewports.size()) - 1; i >= 0; --i)
			queueViewport(nullptr, m_Viewports[i]);
		
		uint32_t numMainViewports = static_cast<uint32_t>(m_QueuedViewports.size());
		for (uint32_t i = 0; i < numMainViewports; ++i)
			updateQueuedViewport(i);

		FIRE_EVENT(RenderTargetUpdate());

		for (uint32_t i = numMainViewports; i < static_cast<uint32_t>(m_QueuedViewports.size()); ++i)
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

		// �������� Views' Render.
		if (!m_Views.empty())
		{
			for (int32_t i = static_cast<int32_t>(m_Views.size() - 1); i >= 0; --i)
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

		Update(evnt.GetTimeStep());
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

	void Renderer::SetDefaultRenderPath(RenderPath* pRenderPath)
	{
		if (pRenderPath)
			m_pDefaultRenderPath = pRenderPath;
	}

	RenderPath* Renderer::GetDefaultRenderPath() const
	{
		// ����ٸ� Cache�� ���ϸ��� �����Ͽ� ���������� �� ����
		return m_pDefaultRenderPath;
	}

	void Renderer::SetDefaultTechnique(Technique* pTechnique)
	{
		if (pTechnique)
			m_pDefaultTechnique = pTechnique;
	}

	Technique* Renderer::GetDefaultTechnique() const
	{
		// ����ٸ� Cache�� ���ϸ��� �����Ͽ� ���������� �� ����
		return m_pDefaultTechnique;
	}

	Viewport* Renderer::GetViewport(uint32_t index)
	{
		return m_Viewports.size() > index ? m_Viewports[index] : nullptr;
	}

	void Renderer::SetViewport(uint32_t index, Viewport* pViewport)
	{
		if (m_Viewports.size() >= index)
			m_Viewports.insert(m_Viewports.begin() + index, pViewport);
		else
			m_Viewports.emplace_back(pViewport);
	}

	// �̿ܿ��� �� ������ �� ���޹޴´�.
	Texture* Renderer::GetScreenBuffer(int32_t width, int32_t height, DXGI_FORMAT format, bool bCubemap)
	{
		// format���� depthstencil ���θ� �Ǻ�

		auto searchKey = static_cast<uint64_t>(format) << 32u | width << 24u | height << 16u;
		
		if (m_ScreenBuffers.find(searchKey) == m_ScreenBuffers.end())
			m_ScreenBufferIndices[searchKey] = 0;

		auto index = m_ScreenBufferIndices[searchKey];

		if (index >= static_cast<uint32_t>(m_ScreenBuffers[searchKey].size()))
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

		auto numViewports = pRenderTarget->GetViewportCount();
		for (uint32_t i = 0; i < numViewports; ++i)
			queueViewport(pRenderTarget, pRenderTarget->GetViewport(i));
	}

	void Renderer::initialize()
	{
		auto pGraphics = GetSubsystem<Graphics>();
		auto pCache = GetSubsystem<ResourceCache>();
		if (!pGraphics || !pGraphics->IsInitialized() || !pCache)
			return;

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

		DV_LOG_ENGINE_TRACE("Renderer �ʱ�ȭ ����");
	}

	void Renderer::queueViewport(Texture* pRenderTarget, Viewport* pViewport)
	{
		if (!pViewport)
			return;

		m_QueuedViewports.emplace_back(std::make_pair(pRenderTarget, pViewport));
	}

	void Renderer::updateQueuedViewport(uint32_t index)
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