#include "Log.h"
#include "External/spdlog/sinks/stdout_color_sinks.h"
#include "External/spdlog/sinks/basic_file_sink.h"

namespace Dive
{
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_appLogger;

	void Log::Initialize()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_coreLogger = std::make_shared<spdlog::logger>("CORE", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_coreLogger);
		s_coreLogger->set_level(spdlog::level::trace);
		s_coreLogger->flush_on(spdlog::level::trace);

		s_appLogger = std::make_shared<spdlog::logger>("APP", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_appLogger);
		s_appLogger->set_level(spdlog::level::trace);
		s_appLogger->flush_on(spdlog::level::trace);

		CORE_INFO("Log System 초기화를 수행하였습니다.");
	}
}