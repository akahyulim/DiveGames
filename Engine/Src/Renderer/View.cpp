#include "divepch.h"
#include "View.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "IO/Log.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Component/StaticModel.h"

namespace Dive 
{
	View::View(Context* pContext)
		: Object(pContext),
		m_pScene(nullptr),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pRenderer(GetSubsystem<Renderer>())
	{
		m_Rect = { 0, 0, 0, 0 };
	}

	View::View(Context* pContext, Scene* pScene)
		: Object(pContext),
		m_pScene(pScene),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pRenderer(GetSubsystem<Renderer>())
	{
		m_Rect = { 0, 0, 0, 0 };
	}

	View::~View()
	{
		DV_LOG_ENGINE_DEBUG("View 소멸자 호출");
	}

	void View::Render()
	{
		//DV_LOG_ENGINE_DEBUG("View::Render()");

		//excuteRenderPathCommands();

		// test render
		{
			DirectX::XMFLOAT4 color{ 0.9f, 0.9f, 0.5f, 1.0f };
			m_pGraphics->Clear(0, color, 0.0f, 0.0f);

			if (!m_pScene->GetGameObjectsCount())
				return;

			// pass를 구성해 그려야 한다.
			// 그럴려면 StaticModel Component를 Scene으로부터 가져와야 한다.
			// 허나 urho의 경우 내부 과정이 좀 더 복잡하다...
			// Scene의 Octree로부터 Drawable, Geometry, SourceBatch 순으로 뽑아내는 듯 하다.
			// Octree자체는 Scene이 가지는 Component다...
			// 하지만 Scene에서 Octree를 다루는 부분이 없다.
			// Drawable을 뽑아낸 후 Octree에 넣어서 그려질 부분을 정제하는 것 같기도 하고...
		}
	}
 
	void View::setRenderTargets(int command)
	{
		// command로부터 정보를 읽어온 후
		// rendertargetview, depthstencilview를 set한다.
		// 이는 결국 command별로 실행된다.

		// graphics에 set rendertarget, set depthstencil 함수를 만들어 두어, 인자를 넘긴다.
		// 하지만 실제 omsetrendertargets는 graphics의 다른 곳에서 호출된다.
	}

	void View::excuteRenderPathCommands()
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