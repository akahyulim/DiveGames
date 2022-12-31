#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Scene;
	class View;
	class RenderPath;

	class Viewport : public Object
	{
		DIVE_OBJECT(Viewport, Object)
	
	public:
		explicit Viewport(Context* pContext);
		Viewport(Context* pContext, Scene* pScene, RenderPath* pRenderPath); //+ camera
		Viewport(Context* pContext, Scene* pScene, RenderPath* pRenderPath, const RECT& rect); // + camera
		~Viewport() override;

		RenderPath* GetRenderPath() const { return m_pRenderPath; }
		void SetRenderPath(RenderPath* pRenderPath);
		// 파일로부터 직접 로드하는 함수도 가진다.

		Scene* GetScene() const { return m_pScene; }
		void SetScene(Scene* pScene) { m_pScene = pScene; }

		RECT GetRect() const { return m_Rect; }
		void SetRect(const RECT& rect) { m_Rect = rect; }

		// camera

		View* GetView() { return m_pView; }

		void AllocateView();

	private:
	private:
		Scene* m_pScene;
		RenderPath* m_pRenderPath;
		View* m_pView;
		RECT m_Rect;
	};
}