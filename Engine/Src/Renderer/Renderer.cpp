#include "divepch.h"
#include "Renderer.h"
#include "View.h"
#include "Core/Context.h"
#include "Core/CoreEvents.h"
#include "IO/Log.h"
#include "Graphics/Graphics.h"
#include "Core/CoreDefs.h"

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
	}
	
	// EventHandler�� ȣ���Ѵ�.
	void Renderer::Update(float delta)
	{
		//DV_LOG_ENGINE_DEBUG("Renderer::Update({:f})", delta);

		// view clear

		// ���� assert graphics �� lost device
	}
	
	void Renderer::Render()
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

	std::shared_ptr<View> Renderer::GetView(unsigned int index)
	{
		return m_Views.size() > index ? m_Views[index] : std::shared_ptr<View>();
	}

	void Renderer::SetView(unsigned int index, std::shared_ptr<View>& view)
	{
		if (m_Views.size() >= index)
			m_Views.insert(m_Views.begin() + index, view);
		else
			m_Views.emplace_back(view);
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
}