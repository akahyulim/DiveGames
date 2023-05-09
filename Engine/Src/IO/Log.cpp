#include "DivePch.h"
#include "Log.h"

namespace Dive
{
	static std::shared_ptr<spdlog::logger> s_pEngineLogger;
	static std::shared_ptr<spdlog::logger> s_pAppLogger;

	void Log::Initialize()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_pEngineLogger = std::make_shared<spdlog::logger>("CORE", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_pEngineLogger);
		s_pEngineLogger->set_level(spdlog::level::trace);
		s_pEngineLogger->flush_on(spdlog::level::trace);

		s_pAppLogger = std::make_shared<spdlog::logger>("APP", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_pAppLogger);
		s_pAppLogger->set_level(spdlog::level::trace);
		s_pAppLogger->flush_on(spdlog::level::trace);
	}
	
	spdlog::logger* Log::GetEngineLogger()
	{
		return s_pEngineLogger.get();
	}

	spdlog::logger* Log::GetAppLogger()
	{
		return s_pAppLogger.get();
	}
}