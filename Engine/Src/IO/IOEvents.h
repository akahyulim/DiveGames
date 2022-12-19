#pragma once
#include "Core/EventSystem.h"

namespace Dive
{
	class LogMessageEvent : public Event 
	{
		EVENT_CLASS_TYPE(LogMessage)

	public:
		LogMessageEvent(uint32_t level, const std::string& message)
			: m_Level(level),
			m_Message(message)
		{}
		~LogMessageEvent() = default;

		uint32_t GetLogLevel() const { return m_Level; }
		std::string GetLogMessage() const { return m_Message; }

	private:
		uint32_t m_Level;
		std::string m_Message;
	};

	class ExitRequestEvent : public Event
	{
		EVENT_CLASS_TYPE(ExitRequested)

	public:
		ExitRequestEvent() = default;
		~ExitRequestEvent() = default;
	};
}