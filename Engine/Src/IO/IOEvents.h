#pragma once
#include "Core/EventSystem.h"

namespace Dive
{
	class LogMessageEvent : public Event 
	{
		EVENT_CLASS_TYPE(LogMessage)

	public:
		LogMessageEvent() = default;
		~LogMessageEvent() = default;

		unsigned int GetLogLevel() const { return m_Level; }
		void SetLogLevel(unsigned int level) { m_Level = level; }

		std::string GetLogMessage() const { return m_Message; }
		void SetLogMessage(const std::string& msg) { m_Message = msg; }

	private:
		unsigned int m_Level = 0;
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