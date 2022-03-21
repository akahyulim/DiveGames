#pragma once
#include <string>
#include <sstream>
#include "EventSystem.h"
#include "Base/Engine.h"

namespace Dive
{
	class WindowDataEvent : public Event
	{
	public:
		WindowDataEvent(const WindowData& data) : m_WindowData(data) {}

		const WindowData& GetWindowData() const { return m_WindowData; }

		EVENT_CLASS_TYPE(WindowData)

	private:
		WindowData m_WindowData;
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height)
		{}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)

	private:
		unsigned int m_Width;
		unsigned int m_Height;
	};
}