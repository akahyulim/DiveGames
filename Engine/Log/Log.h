#pragma once

#include "Core/DiveDefs.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace spdlog { class logger; }

namespace Dive
{
	class DIVE_CLASS Log
	{
	public:
		static void Initialize();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_pCoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_pClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_pCoreLogger;
		static std::shared_ptr<spdlog::logger> s_pClientLogger;
	};
}
	
// Engine Logger Macro
#define ENGINE_TRACE(...)		Dive::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)		Dive::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)		Dive::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)		Dive::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_FATAL(...)		Dive::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Application Logger Macro
#define APP_TRACE(...)			Dive::Log::GetClientLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)			Dive::Log::GetClientLogger()->info(__VA_ARGS__)
#define APP_WARN(...)			Dive::Log::GetClientLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)			Dive::Log::GetClientLogger()->error(__VA_ARGS__)
#define APP_FATAL(...)			Dive::Log::GetClientLogger()->fatal(__VA_ARGS__)