#pragma once
#include "IOEvents.h"
#include "Core/Object.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <sstream>

#define DV_LOG_ENGINE_TRACE(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Trace, __VA_ARGS__)
#define DV_LOG_ENGINE_DEBUG(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Debug, __VA_ARGS__)
#define DV_LOG_ENGINE_INFO(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Info, __VA_ARGS__)
#define DV_LOG_ENGINE_WARN(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Warn, __VA_ARGS__)
#define DV_LOG_ENGINE_ERROR(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Error, __VA_ARGS__)
#define DV_LOG_ENGINE_CRITICAL(...) Dive::Log::Write(Dive::eLoggerNames::Engine, Dive::eLogLevels::Critical, __VA_ARGS__)

#define DV_LOG_CLIENT_TRACE(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Trace, __VA_ARGS__)
#define DV_LOG_CLIENT_DEBUG(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Debug, __VA_ARGS__)
#define DV_LOG_CLIENT_INFO(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Info, __VA_ARGS__)
#define DV_LOG_CLIENT_WARN(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Warn, __VA_ARGS__)
#define DV_LOG_CLIENT_ERROR(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Error, __VA_ARGS__)
#define DV_LOG_CLIENT_CRITICAL(...) Dive::Log::Write(Dive::eLoggerNames::Client, Dive::eLogLevels::Critical, __VA_ARGS__)

namespace Dive
{
	// 로거 타입 열거자.
	enum class eLoggerNames
	{
		Engine,
		Client
	};

	// 로그 레벨 열거자.
	enum class eLogLevels
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	class Log : public Object
	{
		DIVE_OBJECT(Log, Object)

	public:
		explicit Log(Context* pContext)
			: Object(pContext) {}
		~Log() override;

		void Initialize(const char* pFilename);

		// set level

		template<typename FormatString, typename... Args>
		static void Write(eLoggerNames name, eLogLevels level, const FormatString& fmt, const Args &... args)
		{
			if (name == eLoggerNames::Engine)
			{
				switch (level)
				{
				case eLogLevels::Trace:
					s_EngineLogger->trace(fmt, args...);
					break;
				case eLogLevels::Debug:
					s_EngineLogger->debug(fmt, args...);
					break;
				case eLogLevels::Info:
					s_EngineLogger->info(fmt, args...);
					break;
				case eLogLevels::Warn:
					s_EngineLogger->warn(fmt, args...);
					break;
				case eLogLevels::Error:
					s_EngineLogger->error(fmt, args...);
					break;
				case eLogLevels::Critical:
					s_EngineLogger->critical(fmt, args...);
					break;
				}
			}
			else
			{
				switch (level)
				{
				case eLogLevels::Trace:
					s_ClientLogger->trace(fmt, args...);
					break;
				case eLogLevels::Debug:
					s_ClientLogger->debug(fmt, args...);
					break;
				case eLogLevels::Info:
					s_ClientLogger->info(fmt, args...);
					break;
				case eLogLevels::Warn:
					s_ClientLogger->warn(fmt, args...);
					break;
				case eLogLevels::Error:
					s_ClientLogger->error(fmt, args...);
					break;
				case eLogLevels::Critical:
					s_ClientLogger->critical(fmt, args...);
					break;
				}
			}

			FIRE_EVENT(LogMessageEvent(static_cast<uint32_t>(level), s_Oss.str()));
		}

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		static std::ostringstream s_Oss;
	};
}