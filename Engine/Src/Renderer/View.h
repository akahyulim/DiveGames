#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Graphics;
	class Renderer;

	// camera, renderpath�� �̿��� scene�� �׸���.
	class View : public Object
	{
		DIVE_OBJECT(View, Object)

	public:
		explicit View(Context* pContext);
		// �⺻ �Ű�����
		// �⺻ �Ű����� + rect
		~View();

		// view part
		// update: FrameInfo = frame count, time step, view size, camera
		void Render();

		// viewport part
		// get, set scene
		// get, set camera
		// get, set, load render path
		// get, set rect
		// get screen ray
		// world to screen point
		// screen to world point

	private:
		void setRenderTargets(int command);

		void excuteRenderPathCommands();

	private:
		// viewport part
		// scene
		// camera
		// render path
		RECT m_Rect;

		// view part
		Graphics* m_pGraphics;
		Renderer* m_pRenderer;
	};
}