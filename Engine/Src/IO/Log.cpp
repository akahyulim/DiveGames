#include "divepch.h"
#include "Log.h"

namespace Dive
{
	std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::ostringstream Log::s_Oss;

	Log::~Log()
	{
		DV_LOG_ENGINE_TRACE("Log ¼Ò¸ê ¿Ï·á");
	}

	void Log::Initialize(const char* pFilename)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(pFilename, true));
		logSinks.emplace_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(s_Oss));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");
		logSinks[2]->set_pattern("%v");

		s_EngineLogger = std::make_shared<spdlog::logger>("Engine", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_EngineLogger);
		s_EngineLogger->set_level(spdlog::level::trace);
		s_EngineLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("Client", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
}