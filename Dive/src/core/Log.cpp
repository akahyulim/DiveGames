#include "stdafx.h"
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Dive
{
	std::string LogManager::s_filename = "dive.log";
	spdlog::level::level_enum LogManager::s_setLevel = spdlog::level::trace;
	spdlog::level::level_enum LogManager::s_flushLevel = spdlog::level::trace;

	std::shared_ptr<spdlog::logger> LogManager::GetLogger(const std::string& category)
	{
		auto logger = spdlog::get(category);
		
		if (!logger)
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(s_filename, true));

			logSinks[0]->set_pattern("%^[%T] [%n] %v%$");
			logSinks[1]->set_pattern("[%T] [%l] [%n] %v");

			logger = std::make_shared<spdlog::logger>(category, std::begin(logSinks), std::end(logSinks));
			logger->set_level(s_setLevel);
			logger->flush_on(s_flushLevel);
			spdlog::register_logger(logger);
		}

		return logger;
	}
}
