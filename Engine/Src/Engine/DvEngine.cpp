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
	
	bool DvEngine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem 중 graphics, renderer 생성
		m_pContext->RegisterSubsystem(std::make_shared<DvGraphics>(m_pContext));

		// 각종 subsystem 초기화
		auto pLog = m_pContext->GetSubsystem<DvLog>();
		pLog->Initialize("Dive.log");

		// 그래픽스 초기화
		{
			auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
			pGraphics->SetTitle(parameters.title);
			// icon
			pGraphics->SetPosition(parameters.positionX, parameters.positionY);

			if (!pGraphics->SetMode(
				parameters.width,
				parameters.height,
				parameters.bFullscreen,
				parameters.bBorderless,
				parameters.bReSizable,
				parameters.bVSync,
				parameters.bTripleBuffer,
				parameters.multiSample,
				parameters.refreshRate))
			{
				return false;
			}

			// 그래픽스의 기타 설정
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

	bool DvEngine::HasParameter(const VariantMap& parameters, const std::string& parameter)
	{
		return parameters.find(StringHash(parameter)) != parameters.end();
	}

	void DvEngine::doExit()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		if (pGraphics)
		{
			// Graphics::Close()
			// Graphics를 제거하는 건 아니고
			// 윈도우만 종료시킨다.
			// 이유를 아직 잘 모르겠다.
		}

		m_bExiting = true;
	}
}
