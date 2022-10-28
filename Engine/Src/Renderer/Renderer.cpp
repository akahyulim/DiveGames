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
		// ScreenMode 메시지 구독

		// 스크린 모드가 설정되지 않았다면 스킵한단다..
		// 어떻게 스킵하는지 아직 확인 불가
		// 근데 내부에서 Graphics의 초기화를 확인한다.
		// 따라서 이 곳에서는 초기화를 수행할 수 없다.
		initialize();
	}

	Renderer::~Renderer()
	{
		DV_LOG_ENGINE_DEBUG("Renderer 소멸자 호출");

		for (int i = 0; i < static_cast<int>(m_Viewports.size()); ++i)
			DV_DELETE(m_Viewports[i]);
		m_Viewports.clear();
	}
	
	// EventHandler가 호출한다.
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
		// 임시: 현재 initialize() 호출할 곳이 없다.
		// 추후 이를 삭제하고 OnScreenMode로 처리.
		if (!m_bInitialized)
		{
			initialize();
		}

		// assert로 graphics 초기화 및 lost 확인
		// 자신의 initialize도 확인해야 할 것 같은데...
		DV_ASSERT(m_pGraphics || m_pGraphics->IsInitialized() || !m_pGraphics->IsDeviceLost());

		// 역순으로 Views' Render.
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

	// 이게 호출되려면 OnScreenModeChanged나 OnWindowResized가 호출되어야 한다.
	// orho는 SetScreenMode의 마지막에 OnScreenModeChagned를 호출한다.
	// OnScreenModeChanged는 설정을 로그로 출력하고, 데이터를 인자로 이벤트에 전달한다.
	// private인 것으로 보아 이때(SetScreenMode)만 호출되는 듯 하다.
	void Renderer::OnScreenMode(const Event& e)
	{
		if (!m_bInitialized)
			initialize();

		// 이미 생성되었다면 m_bViewReset = true;
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

	// 이외에도 몇 가지를 더 전달받는다.
	Texture* Renderer::GetScreenBuffer(int width, int height, DXGI_FORMAT format, bool bCubemap)
	{
		// format으로 depthstencil 여부를 판별

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
				// depthstencil 여부로 usage 설정.
				pNewTex2D->SetSize(width, height, format);

				pNewBuffer = pNewTex2D;
			}
			else
			{
				// cubemap 생성
			}

			// 기타 설정
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

		// cache manager 도 확인 필요
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		// 일반포인터로 받아 weak_ptr에 넣지 못한다...
		// 실제로 일반 주소값으로는 weak_ptr을 생성할 수 없다고 한다...
		m_pGraphics = pGraphics;

		// cache manager로 부터 load
		// default light ramp
		// default light spot

		// default material 생성

		// 객체를 생성한 후 cache manager로부터 load
		// default render path

		// create geometries
		// create instancing buffer

		// reset shadow map
		// reset buffers

		m_bInitialized = true;

		SUBSCRIBE_EVENT(eEventType::RenderUpdate, EVENT_HANDLER_PARAM(OnRenderUpdate));

		DV_LOG_ENGINE_INFO("Renderer 초기화 성공");
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

		// 원래는 FrameInfo를 전달한다.
		// 그리고 이는 멤버 변수이므로, 여기에서 호출이 가능하다.
		pView->Update(1.0f);
	}
}