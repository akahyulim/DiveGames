#pragma once
#include "spdlog/spdlog.h"

#define DV_CORE_TRACE(...)		Dive::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define DV_CORE_INFO(...)		Dive::Log::GetEngineLogger()->info(__VA_ARGS__)
#define DV_CORE_DEBUG(...)		Dive::Log::GetEngineLogger()->debug(__VA_ARGS__)
#define DV_CORE_WARN(...)		Dive::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define DV_CORE_ERROR(...)		Dive::Log::GetEngineLogger()->error(__VA_ARGS__)
#define DV_CORE_CRITICAL(...)	Dive::Log::GetEngineLogger()->critical(__VA_ARGS__)

#define DV_TRACE(...)		    Dive::Log::GetAppLogger()->trace(__VA_ARGS__)
#define DV_INFO(...)			Dive::Log::GetAppLogger()->info(__VA_ARGS__)
#define DV_DEBUG(...)			Dive::Log::GetAppLogger()->debug(__VA_ARGS__)
#define DV_WARN(...)			Dive::Log::GetAppLogger()->warn(__VA_ARGS__)
#define DV_ERROR(...)			Dive::Log::GetAppLogger()->error(__VA_ARGS__)
#define DV_CRITICAL(...)		Dive::Log::GetAppLogger()->critical(__VA_ARGS__)

namespace Dive
{
	class Log
	{
	public:
		static void Initialize();

		static spdlog::logger* GetEngineLogger();
		static spdlog::logger* GetAppLogger();
	};
}