#include "stdafx.h"
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Dive
{
	std::string LogManager::s_Filename = "dive.log";
	spdlog::level::level_enum LogManager::s_SetLevel = spdlog::level::trace;
	spdlog::level::level_enum LogManager::s_FlushLevel = spdlog::level::trace;

	std::shared_ptr<spdlog::logger> LogManager::GetLogger(const std::string& category)
	{
		auto logger = spdlog::get(category);
		
		if (!logger)
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(s_Filename, true));

			logSinks[0]->set_pattern("%^[%T] [%n] %v%$");
			logSinks[1]->set_pattern("[%T] [%l] [%n] %v");

			logger = std::make_shared<spdlog::logger>(category, std::begin(logSinks), std::end(logSinks));
			logger->set_level(s_SetLevel);
			logger->flush_on(s_FlushLevel);
			spdlog::register_logger(logger);
		}

		return logger;
	}
}
