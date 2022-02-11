#include "divepch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Dive
{
	bool Log::m_bInitialized = false;

	std::shared_ptr<spdlog::logger> Log::m_pCoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_pAppLogger;

	void Log::Initialize()
	{
		if (m_bInitialized)
			return;

		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		m_pCoreLogger = std::make_shared<spdlog::logger>("CORE", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(m_pCoreLogger);
		m_pCoreLogger->set_level(spdlog::level::trace);
		m_pCoreLogger->flush_on(spdlog::level::trace);

		m_pAppLogger = std::make_shared<spdlog::logger>("APP", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(m_pAppLogger);
		m_pAppLogger->set_level(spdlog::level::trace);
		m_pAppLogger->flush_on(spdlog::level::trace);

		m_bInitialized = true;
	}
}