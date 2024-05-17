#include "DivePch.h"
#include "Log.h"

namespace Dive
{
	Log* Log::s_pInstance = nullptr;

	Log::Log()
		: m_pEngineLogger(nullptr)
		, m_pAppLogger(nullptr)
	{
	}

	void Log::Initialize(spdlog::level::level_enum setLevel, spdlog::level::level_enum flushLevel)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		m_pEngineLogger = std::make_shared<spdlog::logger>("ENGINE", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(m_pEngineLogger);
		m_pEngineLogger->set_level(setLevel);
		m_pEngineLogger->flush_on(flushLevel);

		m_pAppLogger = std::make_shared<spdlog::logger>("APP", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(m_pAppLogger);
		m_pAppLogger->set_level(setLevel);
		m_pAppLogger->flush_on(flushLevel);
	}
	
	spdlog::logger* Log::GetEngineLogger()
	{
		return m_pEngineLogger.get();
	}

	spdlog::logger* Log::GetAppLogger()
	{
		return m_pAppLogger.get();
	}
}