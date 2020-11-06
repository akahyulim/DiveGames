#pragma once
#include "DivePch.h"
#include "EventSystem.h"

namespace Dive
{
	class WindowResizeEvent : public IEvent
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height)
		{}

		unsigned int GetWidth() const { return m_width; }
		unsigned int GetHeight() const { return m_height; }

	private:
		unsigned int m_width;
		unsigned int m_height;
	};
}