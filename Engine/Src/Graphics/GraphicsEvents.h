#pragma once
#include "Core/EventSystem.h"
#include <Windows.h>

namespace Dive
{
	// WindowEvent
	class WindowEvent : public Event
	{
		EVENT_CLASS_TYPE(WindowEvent)

	public:
		WindowEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
			: m_hWnd(hWnd),
			m_Msg(msg),
			m_wParam(wParam),
			m_lParam(lParam)
		{}
		~WindowEvent() = default;

		HWND m_hWnd = 0;
		UINT m_Msg = 0;
		WPARAM m_wParam = 0;
		LPARAM m_lParam = 0;
	};

	// ScreenMode
	class ScreenModeEvent : public Event
	{
		EVENT_CLASS_TYPE(ScreenMode)

	public:
		ScreenModeEvent() = default;
		~ScreenModeEvent() = default;
	};
	
	// BeginRender
	class BeginRenderEvent : public Event
	{
		EVENT_CLASS_TYPE(BeginRender)

	public:
		BeginRenderEvent() = default;
		~BeginRenderEvent() = default;
	};

	// EndRender
	class EndRenderEvent : public Event
	{
		EVENT_CLASS_TYPE(EndRender)

	public:
		EndRenderEvent() = default;
		~EndRenderEvent() = default;
	};
}