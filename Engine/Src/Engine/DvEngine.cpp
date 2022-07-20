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
		m_pContext->RegisterSubsystem(this);

		// �⺻ subsystem ����: timer, loger, resource cache, input, filesystem
		m_pContext->RegisterSubsystem(new DvLog(pContext));

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
	}

	DvEngine::~DvEngine()
	{
	}
	
	bool DvEngine::Initialize(const VariantMap& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem �� graphics, renderer ����
		m_pContext->RegisterSubsystem(new DvGraphics(m_pContext));

		// ���� subsystem �ʱ�ȭ
		auto pLog = m_pContext->GetSubsystem<DvLog>();
		pLog->Initialize("Dive.log");

		// �׷��Ƚ� �ʱ�ȭ
		{
			auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
			// ������ ����
			// �׷��Ƚ� ����̽� ����
		}

		DV_LOG_ENGINE_INFO("���� �ʱ�ȭ ����");
		
		m_bInitialized = true;

		return true;
	}
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		Update();

		Render();
	}
	
	void DvEngine::Update()
	{
		// timestep��
		// Update, Post_, Render_, PostRender_Update �� ������ ���� �̺�Ʈ�� ȣ���Ѵ�.
		// ���⿡�� Render_Update�� Rendering�� �϶�� �̺�Ʈ�� �ƴ� �� �ϴ�.
	}
	
	void DvEngine::Render()
	{
		auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
		if (!pGraphics->BeginFrame())
		{
			m_bExiting = true;	// temp
			return;
		}

		// renderer
		// ui

		pGraphics->EndFrame();
	}

	// �׷��Ƚ��� �����츦 Ŭ�����ϰ� exit flag�� �����Ѵܴ�.
	// �����δ� doExit()���� ����ȴ�.
	// Application�� ErrorExit()���� ȣ��ȴ�.
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
		}

		m_bExiting = true;
	}
}
