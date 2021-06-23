#include "Log.h"
#include "External/spdlog/sinks/stdout_color_sinks.h"
#include "External/spdlog/sinks/basic_file_sink.h"

namespace dive
{
	std::shared_ptr<spdlog::logger> Log::sCoreLogger;
	std::shared_ptr<spdlog::logger> Log::sAppLogger;

	void Log::Initialize()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		sCoreLogger = std::make_shared<spdlog::logger>("CORE", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(sCoreLogger);
		sCoreLogger->set_level(spdlog::level::trace);
		sCoreLogger->flush_on(spdlog::level::trace);

		sAppLogger = std::make_shared<spdlog::logger>("APP", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(sAppLogger);
		sAppLogger->set_level(spdlog::level::trace);
		sAppLogger->flush_on(spdlog::level::trace);

		CORE_INFO("Log System 초기화를 수행하였습니다.");
	}
}