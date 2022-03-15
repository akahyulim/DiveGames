#pragma once
#include <string>
#include <sstream>
#include "EventSystem.h"

namespace Dive
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height)
		{}

		static constexpr EventType s_Type = "WindowResizeEvent";

		virtual EventType GetType() const
		{
			return s_Type;
		}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	private:
		unsigned int m_Width;
		unsigned int m_Height;
	};
}