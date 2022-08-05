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
		DV_LOG_ENGINE_DEBUG("DvView 소멸자 호출");
	}

	void DvView::Render()
	{
		DV_LOG_ENGINE_DEBUG("DvView::Render()");

		excuteRenderPathCommands();
	}
 
	void DvView::setRenderTargets(int command)
	{
		// command로부터 정보를 읽어온 후
		// rendertargetview, depthstencilview를 set한다.
		// 이는 결국 command별로 실행된다.

		// graphics에 set rendertarget, set depthstencil 함수를 만들어 두어, 인자를 넘긴다.
		// 하지만 실제 omsetrendertargets는 graphics의 다른 곳에서 호출된다.
	}

	void DvView::excuteRenderPathCommands()
	{
		// renderpath에서 command를 읽은 후 순서대로 실행
		// forward를 예로 들자면
		// clear, scenepass(base), forwardlights, scenepass(postopaque), scenepass(refract),
		// scenepass(alpha), scenepass(postalpha) 순이다.
		// 결국 renderpath는 pass의 묶음이다.


		// command별 switch
		{
			// Clear: ClearRenderTargets
			{
				setRenderTargets(0);

				// rendertarget을 set한 후 color로 clear
				DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 0.0f, 1.0f };
				m_pGraphics->Clear(0, color, 0.0f, 0.0f);
			}

			// ScenePass
			{
				// batch queue의 draw를 한다.
				// 직접 game object를 renderable별로 다루진 않는다.
				// 결국 batch queue를 어디에서 어떻게 만드느냐 알아야 한다.
			}

			// Quad

			// ForwardLights
			{
				// forawrd에서 사용하는 light
			}

			// LightVolumes
			{
				// deferred에서 사용하는 light
			}
		}
	}
}