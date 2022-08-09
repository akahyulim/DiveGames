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
		//DV_LOG_ENGINE_DEBUG("Renderer::Update({:f})", delta);

		// view clear

		// ���� assert graphics �� lost device
	}
	
	void DvRenderer::Render()
	{
		// �ӽ�: ���� initialize() ȣ���� ���� ����.
		// ���� �̸� �����ϰ� OnScreenMode�� ó��.
		if (!m_bInitialized)
		{
			initialize();
		}

		//DV_LOG_ENGINE_DEBUG("Rendrer::Render()");

		// assert�� graphics �ʱ�ȭ �� lost Ȯ��
		// �ڽ��� initialize�� Ȯ���ؾ� �� �� ������...
		DV_ASSERT(m_pGraphics || m_pGraphics->IsInitialized() || !m_pGraphics->IsDeviceLost());

		// �ᱹ View��� ���� Render()�� �����Ѵ�.
		// �������� �׸���.
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

	// �̰� ȣ��Ƿ��� OnScreenModeChanged�� OnWindowResized�� ȣ��Ǿ�� �Ѵ�.
	// orho�� SetScreenMode�� �������� OnScreenModeChagned�� ȣ���Ѵ�.
	// OnScreenModeChanged�� ������ �α׷� ����ϰ�, �����͸� ���ڷ� �̺�Ʈ�� �����Ѵ�.
	// private�� ������ ���� �̶�(SetScreenMode)�� ȣ��Ǵ� �� �ϴ�.
	void DvRenderer::OnScreenMode(const Variant& var)
	{
		if (!m_bInitialized)
			initialize();

		// �̹� �����Ǿ��ٸ� m_bViewReset = true;
	}

	void DvRenderer::initialize()
	{
		auto pGraphics = GetSubsystem<DvGraphics>();
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

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::RenderUpdate, DV_EVENT_HANDLER_VARIANT(OnRenderUpdate));
		
		DV_LOG_ENGINE_INFO("Renderer �ʱ�ȭ ����");
	}
}