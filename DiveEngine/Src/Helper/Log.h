#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace dive
{
	class Log
	{
	public:
		static void Initialize();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetAppLogger() { return sAppLogger; }

	private:
		static std::shared_ptr<spdlog::logger> sCoreLogger;
		static std::shared_ptr<spdlog::logger> sAppLogger;
	};
}

// Engine Logger Macro
#define CORE_TRACE(...)			dive::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)			dive::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)			dive::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)			dive::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)		dive::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Application Logger Macro
#define APP_TRACE(...)			dive::Log::GetAppLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)			dive::Log::GetAppLogger()->info(__VA_ARGS__)
#define APP_WARN(...)			dive::Log::GetAppLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)			dive::Log::GetAppLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)		dive::Log::GetAppLogger()->critical(__VA_ARGS__)