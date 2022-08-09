#include "divepch.h"
#include "DvRenderer.h"
#include "DvView.h"
#include "Core/DvContext.h"
#include "IO/DvLog.h"
#include "Graphics/DvGraphics.h"
#include "Base/Base.h"

namespace Dive
{
	DvRenderer::DvRenderer(DvContext* pContext)
		: DvObject(pContext),
		m_pGraphics(nullptr),
		m_bInitialized(false)
	{
		// E_SCREENMODE, HandleScreenMode 메시지 구독

		// 스크린 모드가 설정되지 않았다면 스킵한단다..
		// 어떻게 스킵하는지 아직 확인 불가
		// 근데 내부에서 Graphics의 초기화를 확인한다.
		// 따라서 이 곳에서는 초기화를 수행할 수 없다.
		initialize();
	}

	DvRenderer::~DvRenderer()
	{
		DV_LOG_ENGINE_DEBUG("Renderer 소멸자 호출");
	}
	
	// EventHandler가 호출한다.
	void DvRenderer::Update(float delta)
	{
		//DV_LOG_ENGINE_DEBUG("Renderer::Update({:f})", delta);

		// view clear

		// 역시 assert graphics 및 lost device
	}
	
	void DvRenderer::Render()
	{
		// 임시: 현재 initialize() 호출할 곳이 없다.
		// 추후 이를 삭제하고 OnScreenMode로 처리.
		if (!m_bInitialized)
		{
			initialize();
		}

		//DV_LOG_ENGINE_DEBUG("Rendrer::Render()");

		// assert로 graphics 초기화 및 lost 확인
		// 자신의 initialize도 확인해야 할 것 같은데...
		DV_ASSERT(m_pGraphics || m_pGraphics->IsInitialized() || !m_pGraphics->IsDeviceLost());

		// 결국 View라는 것의 Render()를 수행한다.
		// 역순으로 그린다.
		for (int i = (int)m_Views.size() - 1; i >= 0; --i)
		{
			if (!m_Views[i])
				continue;

			// prepareViewRender()
			m_Views[i]->Render();
		}
	}

	void DvRenderer::OnRenderUpdate(const Variant& data)
	{
		auto deltaTime = data.Get<float>();
		Update(deltaTime);
	}

	std::shared_ptr<DvView> DvRenderer::GetView(unsigned int index)
	{
		return m_Views.size() > index ? m_Views[index] : std::shared_ptr<DvView>();
	}

	void DvRenderer::SetView(unsigned int index, std::shared_ptr<DvView>& view)
	{
		if (m_Views.size() >= index)
			m_Views.insert(m_Views.begin() + index, view);
		else
			m_Views.emplace_back(view);
	}

	// 이게 호출되려면 OnScreenModeChanged나 OnWindowResized가 호출되어야 한다.
	// orho는 SetScreenMode의 마지막에 OnScreenModeChagned를 호출한다.
	// OnScreenModeChanged는 설정을 로그로 출력하고, 데이터를 인자로 이벤트에 전달한다.
	// private인 것으로 보아 이때(SetScreenMode)만 호출되는 듯 하다.
	void DvRenderer::OnScreenMode(const Variant& var)
	{
		if (!m_bInitialized)
			initialize();

		// 이미 생성되었다면 m_bViewReset = true;
	}

	void DvRenderer::initialize()
	{
		auto pGraphics = GetSubsystem<DvGraphics>();
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

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::RenderUpdate, DV_EVENT_HANDLER_VARIANT(OnRenderUpdate));
		
		DV_LOG_ENGINE_INFO("Renderer 초기화 성공");
	}
}