#pragma once

#include <spdlog/spdlog.h>

namespace spdlog { class logger; }

namespace Dive
{
	class Log
	{
	public:
		static void Initialize();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetAppLogger()	{ return s_AppLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_AppLogger;
	};
}
	
// Engine Logger Macro
#define CORE_TRACE(...)			Dive::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)			Dive::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)			Dive::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)			Dive::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)		Dive::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Application Logger Macro
#define APP_TRACE(...)			Dive::Log::GetAppLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)			Dive::Log::GetAppLogger()->info(__VA_ARGS__)
#define APP_WARN(...)			Dive::Log::GetAppLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)			Dive::Log::GetAppLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)		Dive::Log::GetAppLogger()->critical(__VA_ARGS__)