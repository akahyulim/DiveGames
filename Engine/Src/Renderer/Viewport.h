#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Scene;
	class View;

	enum class eRenderPath
	{
		Forward,
		Deferred,
	};

	class Viewport : public Object
	{
		DIVE_OBJECT(Viewport, Object)
	
	public:
		explicit Viewport(Context* pContext);
		Viewport(Context* pContext, Scene* pScene, eRenderPath renderPath); //+ camera
		Viewport(Context* pContext, Scene* pScene, const RECT& rect, eRenderPath renderPath); // + camera
		~Viewport() override;

		Scene* GetScene() const { return m_pScene; }
		void SetScene(Scene* pScene) { m_pScene = pScene; }

		RECT GetRect() const { return m_Rect; }
		void SetRect(const RECT& rect) { m_Rect = rect; }

		eRenderPath GetRenderPath() const { return m_RenderPath; }
		void SetRenderPath(eRenderPath renderPath) { m_RenderPath = renderPath; }

		// camera

		View* GetView() { return m_pView; }

		void AllocateView();

	private:
	private:
		Scene* m_pScene = nullptr;
		RECT m_Rect;
		eRenderPath m_RenderPath;
		View* m_pView = nullptr;
	};
}