#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Scene;
	class View;
	class Camera;
	class RenderPath;

	class Viewport : public Object
	{
		DIVE_OBJECT(Viewport, Object)
	
	public:
		explicit Viewport(Context* pContext);
		Viewport(Context* pContext, Scene* pScene, Camera* pCamera, RenderPath* pRenderPath);
		Viewport(Context* pContext, Scene* pScene, Camera* pCamera, RenderPath* pRenderPath, const RECT& rect);
		~Viewport() override;

		RenderPath* GetRenderPath() const { return m_pRenderPath; }
		void SetRenderPath(RenderPath* pRenderPath);
		// ���Ϸκ��� ���� �ε��ϴ� �Լ��� ������.

		Scene* GetScene() const { return m_pScene; }
		void SetScene(Scene* pScene) { m_pScene = pScene; }

		RECT GetRect() const { return m_Rect; }
		void SetRect(const RECT& rect) { m_Rect = rect; }

		Camera* GetCamera() const { return m_pCamera; }
		void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

		View* GetView() { return m_pView; }

		// ���� ī�޶� Ȱ���ϴ� �Լ�
		// GetScreenRay
		// worldToScreen
		// screenToWorld

		void AllocateView();

	private:
	private:
		Scene* m_pScene;
		RenderPath* m_pRenderPath;
		View* m_pView;
		Camera* m_pCamera;
		RECT m_Rect;
	};
}