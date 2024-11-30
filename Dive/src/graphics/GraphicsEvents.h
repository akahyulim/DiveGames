#pragma once
#include "Core/EventHandler.h"
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

		HWND m_hWnd;
		UINT m_Msg;
		WPARAM m_wParam;
		LPARAM m_lParam;
	};

	// ScreenMode
	class ScreenMode : public Event
	{
		EVENT_CLASS_TYPE(ScreenMode)

	public:
		ScreenMode() = default;
		~ScreenMode() = default;

	private:
	};

	/*
	// BeginRender
	class BeginRender : public Event
	{
		EVENT_CLASS_TYPE(BeginRender)

	public:
		BeginRender() = default;
		~BeginRender() = default;

	private:
	};

	// EndRender
	class EndRender : public Event
	{
		EVENT_CLASS_TYPE(EndRender)

	public:
		EndRender() = default;
		~EndRender() = default;

	private:
	};
	*/
}