#include "divepch.h"
#include "DvView.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"
#include "IO/DvLog.h"
#include "Graphics/DvGraphics.h"
#include "Renderer/DvRenderer.h"

namespace Dive 
{
	DvView::DvView(DvContext* pContext)
		: DvObject(pContext),
		m_pGraphics(GetSubsystem<DvGraphics>()),
		m_pRenderer(GetSubsystem<DvRenderer>())
	{
		m_Rect = { 0, 0, 0, 0 };
	}

	DvView::~DvView()
	{
		DV_LOG_ENGINE_DEBUG("DvView �Ҹ��� ȣ��");
	}

	void DvView::Render()
	{
		DV_LOG_ENGINE_DEBUG("DvView::Render()");

		excuteRenderPathCommands();
	}
 
	void DvView::setRenderTargets(int command)
	{
		// command�κ��� ������ �о�� ��
		// rendertargetview, depthstencilview�� set�Ѵ�.
		// �̴� �ᱹ command���� ����ȴ�.

		// graphics�� set rendertarget, set depthstencil �Լ��� ����� �ξ�, ���ڸ� �ѱ��.
		// ������ ���� omsetrendertargets�� graphics�� �ٸ� ������ ȣ��ȴ�.
	}

	void DvView::excuteRenderPathCommands()
	{
		// renderpath���� command�� ���� �� ������� ����
		// forward�� ���� ���ڸ�
		// clear, scenepass(base), forwardlights, scenepass(postopaque), scenepass(refract),
		// scenepass(alpha), scenepass(postalpha) ���̴�.
		// �ᱹ renderpath�� pass�� �����̴�.


		// command�� switch
		{
			// Clear: ClearRenderTargets
			{
				setRenderTargets(0);

				// rendertarget�� set�� �� color�� clear
				DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 0.0f, 1.0f };
				m_pGraphics->Clear(0, color, 0.0f, 0.0f);
			}

			// ScenePass
			{
				// batch queue�� draw�� �Ѵ�.
				// ���� game object�� renderable���� �ٷ��� �ʴ´�.
				// �ᱹ batch queue�� ��𿡼� ��� ������� �˾ƾ� �Ѵ�.
			}

			// Quad

			// ForwardLights
			{
				// forawrd���� ����ϴ� light
			}

			// LightVolumes
			{
				// deferred���� ����ϴ� light
			}
		}
	}
}