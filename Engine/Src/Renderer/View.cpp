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
		DV_LOG_ENGINE_DEBUG("View �Ҹ��� ȣ��");
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

			// pass�� ������ �׷��� �Ѵ�.
			// �׷����� StaticModel Component�� Scene���κ��� �����;� �Ѵ�.
			// �㳪 urho�� ��� ���� ������ �� �� �����ϴ�...
			// Scene�� Octree�κ��� Drawable, Geometry, SourceBatch ������ �̾Ƴ��� �� �ϴ�.
			// Octree��ü�� Scene�� ������ Component��...
			// ������ Scene���� Octree�� �ٷ�� �κ��� ����.
			// Drawable�� �̾Ƴ� �� Octree�� �־ �׷��� �κ��� �����ϴ� �� ���⵵ �ϰ�...
		}
	}
 
	void View::setRenderTargets(int command)
	{
		// command�κ��� ������ �о�� ��
		// rendertargetview, depthstencilview�� set�Ѵ�.
		// �̴� �ᱹ command���� ����ȴ�.

		// graphics�� set rendertarget, set depthstencil �Լ��� ����� �ξ�, ���ڸ� �ѱ��.
		// ������ ���� omsetrendertargets�� graphics�� �ٸ� ������ ȣ��ȴ�.
	}

	void View::excuteRenderPathCommands()
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