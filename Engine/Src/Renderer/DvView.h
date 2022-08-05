#pragma once
#include "Core/DvObject.h"

namespace Dive 
{
	class DvGraphics;
	class DvRenderer;

	// camera, renderpath를 이용해 scene을 그린다.
	class DvView : public DvObject
	{
		DIVE_OBJECT(DvView, DvObject)

	public:
		explicit DvView(DvContext* pContext);
		// 기본 매개변수
		// 기본 매개변수 + rect
		~DvView();

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
		DvGraphics* m_pGraphics;
		DvRenderer* m_pRenderer;
	};
}