#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace DiveEngine
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
#define CORE_TRACE(...)			DiveEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)			DiveEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)			DiveEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)			DiveEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)		DiveEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Application Logger Macro
#define APP_TRACE(...)			DiveEngine::Log::GetAppLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)			DiveEngine::Log::GetAppLogger()->info(__VA_ARGS__)
#define APP_WARN(...)			DiveEngine::Log::GetAppLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)			DiveEngine::Log::GetAppLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)		DiveEngine::Log::GetAppLogger()->critical(__VA_ARGS__)