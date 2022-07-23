#include "divepch.h"
#include "DvEngine.h"
#include "DvEngineDef.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"
#include "Graphics/DvGraphics.h"
#include "IO/DvLog.h"

namespace Dive
{
	DvEngine::DvEngine(DvContext* pContext)
		: DvObject(pContext),
		m_bInitialized(false),
		m_bExiting(false)
	{
		// 기본 subsystem 생성: timer, loger, resource cache, input, filesystem
		m_pContext->RegisterSubsystem(std::make_shared<DvLog>(pContext));

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
	}

	DvEngine::~DvEngine()
	{
		DV_LOG_ENGINE_DEBUG("DvEngine 소멸자 호출");
	}
	
	bool DvEngine::Initialize(const VariantMap& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem 중 graphics, renderer 생성
		m_pContext->RegisterSubsystem(std::make_shared<DvGraphics>(m_pContext));

		// 각종 subsystem 초기화
		auto pLog = m_pContext->GetSubsystem<DvLog>();
		pLog->Initialize("Dive.log");

		// 그래픽스 초기화
		// 각종 값들은 parameters로부터 얻는다.
		{
			auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
			// 윈도우 생성
			// title
			// icon
			// position
			// setmode: size, full screen, vsync, triple buffers, multi sample, refresh rate
			if (!pGraphics->SetMode(1600, 900, false, false, false, false))
			{
				// graphics도 제거?
				return false;
			}


			// 그래픽스 디바이스 생성
		}

		DV_LOG_ENGINE_INFO("엔진 초기화 성공");
		
		m_bInitialized = true;

		return true;
	}
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		// temp
		// 임시이긴 하지만 구조상 깔끔하다...
		auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
		if (!pGraphics->RunWindow())
		{
			m_bExiting = true;
			return;
		}

		Update();

		Render();
	}
	
	void DvEngine::Update()
	{
		// timestep을
		// Update, Post_, Render_, PostRender_Update 네 갈래로 나뉜 이벤트로 호출한다.
		// 여기에서 Render_Update는 Rendering을 하라는 이벤트는 아닌 듯 하다.
	}
	
	void DvEngine::Render()
	{
		auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
		if (!pGraphics->BeginFrame())
			return;

		// renderer
		// ui

		pGraphics->EndFrame();
	}

	// 그래픽스의 윈도우를 클로즈하고 exit flag를 설정한단다.
	// 실제로는 doExit()에서 수행된다.
	// Application의 ErrorExit()에서 호출된다.
	void DvEngine::Exit()
	{
		doExit();
	}

	void DvEngine::OnExitRequested()
	{
		doExit();
	}

	void DvEngine::doExit()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		if (pGraphics)
		{
			// close
			// Graphics를 제거하는 건 아니고
			// 윈도우만 종료시킨다.
		}

		m_bExiting = true;
	}
}
