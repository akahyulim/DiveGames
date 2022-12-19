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

		HWND GetWindowHandle() const { return m_hWnd; }
		UINT GetWindowMsg() const { return m_Msg; }
		WPARAM GetWindowWParam() const { return m_wParam; }
		LPARAM GetWindowLParam() const { return m_lParam; }

	private:
		HWND m_hWnd;
		UINT m_Msg;
		WPARAM m_wParam;
		LPARAM m_lParam;
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