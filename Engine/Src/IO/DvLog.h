#pragma once
#include "IOEvents.h"
#include "Core/DvObject.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <sstream>

#define DV_LOG_ENGINE_TRACE(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Trace, __VA_ARGS__)
#define DV_LOG_ENGINE_DEBUG(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Debug, __VA_ARGS__)
#define DV_LOG_ENGINE_INFO(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Info, __VA_ARGS__)
#define DV_LOG_ENGINE_WRAN(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Warn, __VA_ARGS__)
#define DV_LOG_ENGINE_ERROR(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Error, __VA_ARGS__)
#define DV_LOG_ENGINE_CRITICAL(...) Dive::DvLog::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Critical, __VA_ARGS__)

#define DV_LOG_CLIENT_TRACE(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Trace, __VA_ARGS__)
#define DV_LOG_CLIENT_DEBUG(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Debug, __VA_ARGS__)
#define DV_LOG_CLIENT_INFO(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Info, __VA_ARGS__)
#define DV_LOG_CLIENT_WARN(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Warn, __VA_ARGS__)
#define DV_LOG_CLIENT_ERROR(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Error, __VA_ARGS__)
#define DV_LOG_CLIENT_CRITICAL(...) Dive::DvLog::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Critical, __VA_ARGS__)

namespace Dive
{
	enum class eLoggerNames
	{
		Engine,
		Client
	};

	enum class eLogLevels
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	class DvLog : public DvObject
	{
		DIVE_OBJECT(DvLog, DvObject)

	public:
		explicit DvLog(DvContext* pContext)
			: DvObject(pContext) {}
		~DvLog() = default;

		void Initialize(const char* filename);

		// set level

		template<typename FormatString, typename... Args>
		static void Write(eLoggerNames name, eLogLevels type, const FormatString& fmt, const Args &... args)
		{
			if (name == eLoggerNames::Engine)
			{
				switch (type)
				{
				case eLogLevels::Trace:
					s_pEngineLogger->trace(fmt, args...);
					break;
				case eLogLevels::Debug:
					s_pEngineLogger->debug(fmt, args...);
					break;
				case eLogLevels::Info:
					s_pEngineLogger->info(fmt, args...);
					break;
				case eLogLevels::Warn:
					s_pEngineLogger->warn(fmt, args...);
					break;
				case eLogLevels::Error:
					s_pEngineLogger->error(fmt, args...);
					break;
				case eLogLevels::Critical:
					s_pEngineLogger->critical(fmt, args...);
					break;
				}
			}
			else
			{
				switch (type)
				{
				case eLogLevels::Trace:
					s_pClientLogger->trace(fmt, args...);
					break;
				case eLogLevels::Debug:
					s_pClientLogger->debug(fmt, args...);
					break;
				case eLogLevels::Info:
					s_pClientLogger->info(fmt, args...);
					break;
				case eLogLevels::Warn:
					s_pClientLogger->warn(fmt, args...);
					break;
				case eLogLevels::Error:
					s_pClientLogger->error(fmt, args...);
					break;
				case eLogLevels::Critical:
					s_pClientLogger->critical(fmt, args...);
					break;
				}
			}

			LogMessageEvent e;
			e.SetLogLevel((unsigned int)type);
			e.SetLogMessage(s_Oss.str());
			DV_FIRE_EVENT(e);
		}

	private:
		static std::shared_ptr<spdlog::logger> s_pEngineLogger;
		static std::shared_ptr<spdlog::logger> s_pClientLogger;

		static std::ostringstream s_Oss;
	};
}