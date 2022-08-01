#include "divepch.h"
#include "DvRenderer.h"
#include "Core/DvContext.h"
#include "IO/DvLog.h"
#include "Graphics/DvGraphics.h"
#include "Base/Base.h"

namespace Dive
{
	DvRenderer::DvRenderer(DvContext* pContext)
		: DvObject(pContext),
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
		DV_LOG_ENGINE_DEBUG("Renderer::Update({:f})", delta);

		// view clear

		// 역시 assert graphics 및 lost device
	}
	
	void DvRenderer::Render()
	{
		// 임시: 현재 initialize() 호출할 곳이 없다.
		if (!m_bInitialized)
		{
			initialize();
		}

		DV_LOG_ENGINE_DEBUG("Rendrer::Render()");

		// assert로 graphics 초기화 및 lost 확인
		// 자신의 initialize도 확인해야 할 것 같은데...
		auto* pGraphics = GetSubsystem<DvGraphics>();	// 추후 weak_ptr로 변경
		DV_ASSERT(pGraphics || pGraphics->IsInitialized() || !pGraphics->IsDeviceLost());

		// 결국 View라는 것의 Render()를 수행한다.
	}

	void DvRenderer::OnRenderUpdate(const Variant& data)
	{
		auto deltaTime = data.Get<float>();
		Update(deltaTime);
	}

	void DvRenderer::initialize()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		// cache manager

		// cache manager 도 확인 필요
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		//m_pGraphics = pGraphics;

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

		//DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
		DV_SUBSCRIBE_TO_EVENT(eDvEventType::RenderUpdate, DV_EVENT_HANDLER_VARIANT(OnRenderUpdate));
		
		DV_LOG_ENGINE_INFO("Renderer 초기화 성공");
	}
}