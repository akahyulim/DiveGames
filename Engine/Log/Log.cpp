#include "DivePch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Dive
{
	std::shared_ptr<spdlog::logger> Log::s_pCoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_pClientLogger;

	void Log::Initialize()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Dive.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_pCoreLogger = std::make_shared<spdlog::logger>("Dive", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_pCoreLogger);
		s_pCoreLogger->set_level(spdlog::level::trace);
		s_pCoreLogger->flush_on(spdlog::level::trace);

		s_pClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_pClientLogger);
		s_pClientLogger->set_level(spdlog::level::trace);
		s_pClientLogger->flush_on(spdlog::level::trace);
		
		/*
		spdlog::set_pattern("%^[%T] %n: %v%$");

//#ifdef DIVE_DEBUG
//		s_pCoreLogger = spdlog::stdout_color_mt("ENGINE");
//		s_pClientLogger = spdlog::stdout_color_mt("APP");
//#else
//		auto dailySync = make_shared<spdlog::sinks::daily_file_sink_mt>("logfile.txt", 23, 59);

//		s_pCoreLogger = make_shared<spdlog::logger>("ENGINE", dailySync);
//		s_pClientLogger = make_shared<spdlog::logger>("APP", dailySync);
//#endif

		s_pCoreLogger->set_level(spdlog::level::trace);
		s_pClientLogger->set_level(spdlog::level::trace);
		*/
	}
}