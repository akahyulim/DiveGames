#include "Core/LogManager.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Dive
{
	namespace
	{
		std::string FILE_NAME = "dive.log";
		spdlog::level::level_enum SET_LEVEL = spdlog::level::trace;
		spdlog::level::level_enum FLUSH_LEVEL = spdlog::level::err;

		static spdlog::level::level_enum ConvertLevel(eLogLevel level)
		{
			switch (level)
			{
			case eLogLevel::trace:    return spdlog::level::trace;
			case eLogLevel::debug:    return spdlog::level::debug;
			case eLogLevel::info:     return spdlog::level::info;
			case eLogLevel::warn:     return spdlog::level::warn;
			case eLogLevel::err:      return spdlog::level::err;
			case eLogLevel::critical: return spdlog::level::critical;
			case eLogLevel::off:      return spdlog::level::off;
			}
			return spdlog::level::info;
		}
	}

	void LogManager::Shutdown()
	{
		spdlog::shutdown();
	}

	void LogManager::SetLevels(eLogLevel setLevel, eLogLevel flushLevel)
	{
		SET_LEVEL = ConvertLevel(setLevel);
		FLUSH_LEVEL = ConvertLevel(flushLevel);
	}

	void LogManager::SetFilename(const std::string& filename)
	{
		FILE_NAME = filename;
	}

	const std::string& LogManager::GetFilename()
	{
		return FILE_NAME;
	}

	std::shared_ptr<spdlog::logger> GetOrCreateLogger(const std::string& category)
	{
		auto logger = spdlog::get(category);
		if (!logger)
		{
			static std::vector<spdlog::sink_ptr> sharedSinks =
			{
				std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
				std::make_shared<spdlog::sinks::daily_file_sink_mt>(FILE_NAME, 0, 0)
			};

			sharedSinks[0]->set_pattern("%^[%T] [%n] %v%$");
			sharedSinks[1]->set_pattern("[%T] [%l] [%n] %v");

			logger = std::make_shared<spdlog::logger>(category, std::begin(sharedSinks), std::end(sharedSinks));
			logger->set_level(SET_LEVEL);
			logger->flush_on(FLUSH_LEVEL);
			spdlog::register_logger(logger);
		}

		return logger;
	}

	void LogManager::OutputInternal(const char* category, eLogLevel level, const std::string& message)
	{
		auto logger = GetOrCreateLogger(category);
		logger->log(ConvertLevel(level), "{}", message);
	}
}