#include "divepch.h"
#include "DvLog.h"

namespace Dive
{
	std::shared_ptr<spdlog::logger> DvLog::s_pEngineLogger;
	std::shared_ptr<spdlog::logger> DvLog::s_pClientLogger;

	std::ostringstream DvLog::s_Oss;

	void DvLog::Initialize(const char* filename)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true));
		logSinks.emplace_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(s_Oss));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");
		logSinks[2]->set_pattern("%v%$");

		s_pEngineLogger = std::make_shared<spdlog::logger>("Engine", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_pEngineLogger);
		s_pEngineLogger->set_level(spdlog::level::trace);
		s_pEngineLogger->flush_on(spdlog::level::trace);

		s_pClientLogger = std::make_shared<spdlog::logger>("Client", std::begin(logSinks), std::end(logSinks));
		spdlog::register_logger(s_pClientLogger);
		s_pClientLogger->set_level(spdlog::level::trace);
		s_pClientLogger->flush_on(spdlog::level::trace);

		//s_pEngineLogger->trace()
	}
}