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
		// E_SCREENMODE, HandleScreenMode �޽��� ����

		// ��ũ�� ��尡 �������� �ʾҴٸ� ��ŵ�Ѵܴ�..
		// ��� ��ŵ�ϴ��� ���� Ȯ�� �Ұ�
		// �ٵ� ���ο��� Graphics�� �ʱ�ȭ�� Ȯ���Ѵ�.
		// ���� �� �������� �ʱ�ȭ�� ������ �� ����.
		initialize();
	}

	DvRenderer::~DvRenderer()
	{
		DV_LOG_ENGINE_DEBUG("Renderer �Ҹ��� ȣ��");
	}
	
	// EventHandler�� ȣ���Ѵ�.
	void DvRenderer::Update(float delta)
	{
		DV_LOG_ENGINE_DEBUG("Renderer::Update({:f})", delta);

		// view clear

		// ���� assert graphics �� lost device
	}
	
	void DvRenderer::Render()
	{
		// �ӽ�: ���� initialize() ȣ���� ���� ����.
		if (!m_bInitialized)
		{
			initialize();
		}

		DV_LOG_ENGINE_DEBUG("Rendrer::Render()");

		// assert�� graphics �ʱ�ȭ �� lost Ȯ��
		// �ڽ��� initialize�� Ȯ���ؾ� �� �� ������...
		auto* pGraphics = GetSubsystem<DvGraphics>();	// ���� weak_ptr�� ����
		DV_ASSERT(pGraphics || pGraphics->IsInitialized() || !pGraphics->IsDeviceLost());

		// �ᱹ View��� ���� Render()�� �����Ѵ�.
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

		// cache manager �� Ȯ�� �ʿ�
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		//m_pGraphics = pGraphics;

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

		//DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
		DV_SUBSCRIBE_TO_EVENT(eDvEventType::RenderUpdate, DV_EVENT_HANDLER_VARIANT(OnRenderUpdate));
		
		DV_LOG_ENGINE_INFO("Renderer �ʱ�ȭ ����");
	}
}