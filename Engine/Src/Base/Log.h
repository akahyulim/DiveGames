#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "Base.h"

namespace Dive
{
	class Log
	{
	public:
		static void Initialize();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() 
		{
			DV_ASSERT(m_bInitialized);
			return m_pCoreLogger; 
		}

		static std::shared_ptr<spdlog::logger>& GetAppLogger() 
		{ 
			DV_ASSERT(m_bInitialized);
			return m_pAppLogger; 
		}

	private:
		static bool m_bInitialized;

		static std::shared_ptr<spdlog::logger> m_pCoreLogger;
		static std::shared_ptr<spdlog::logger> m_pAppLogger;
	};
}

// Engine Logger Macro
#define DIVE_CORE_TRACE(...)		Dive::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DIVE_CORE_INFO(...)			Dive::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DIVE_CORE_WARN(...)			Dive::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DIVE_CORE_ERROR(...)		Dive::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DIVE_CORE_CRITICAL(...)		Dive::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Application Logger Macro
#define DIVE_APP_TRACE(...)			Dive::Log::GetAppLogger()->trace(__VA_ARGS__)
#define DIVE_APP_INFO(...)			Dive::Log::GetAppLogger()->info(__VA_ARGS__)
#define DIVE_APP_WARN(...)			Dive::Log::GetAppLogger()->warn(__VA_ARGS__)
#define DIVE_APP_ERROR(...)			Dive::Log::GetAppLogger()->error(__VA_ARGS__)
#define DIVE_APP_CRITICAL(...)		Dive::Log::GetAppLogger()->critical(__VA_ARGS__)