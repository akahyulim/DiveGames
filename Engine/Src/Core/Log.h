#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#define DV_ENGINE_TRACE(...)	Dive::Log::GetInstance()->GetEngineLogger()->trace(__VA_ARGS__)
#define DV_ENGINE_INFO(...)		Dive::Log::GetInstance()->GetEngineLogger()->info(__VA_ARGS__)
#define DV_ENGINE_DEBUG(...)	Dive::Log::GetInstance()->GetEngineLogger()->debug(__VA_ARGS__)
#define DV_ENGINE_WARN(...)		Dive::Log::GetInstance()->GetEngineLogger()->warn(__VA_ARGS__)
#define DV_ENGINE_ERROR(...)	Dive::Log::GetInstance()->GetEngineLogger()->error(__VA_ARGS__)
#define DV_ENGINE_CRITICAL(...)	Dive::Log::GetInstance()->GetEngineLogger()->critical(__VA_ARGS__)

#define DV_TRACE(...)		    Dive::Log::GetInstance()->GetAppLogger()->trace(__VA_ARGS__)
#define DV_INFO(...)			Dive::Log::GetInstance()->GetAppLogger()->info(__VA_ARGS__)
#define DV_DEBUG(...)			Dive::Log::GetInstance()->GetAppLogger()->debug(__VA_ARGS__)
#define DV_WARN(...)			Dive::Log::GetInstance()->GetAppLogger()->warn(__VA_ARGS__)
#define DV_ERROR(...)			Dive::Log::GetInstance()->GetAppLogger()->error(__VA_ARGS__)
#define DV_CRITICAL(...)		Dive::Log::GetInstance()->GetAppLogger()->critical(__VA_ARGS__)

namespace Dive
{
	class Log
	{
	public:
		Log(const Log&) = delete;
		void operator=(const Log&) = delete;

		static Log* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Log;

			return s_pInstance;
		}

		void Initialize(spdlog::level::level_enum setLevel = spdlog::level::err, spdlog::level::level_enum flushLevel = spdlog::level::err);

		spdlog::logger* GetEngineLogger();
		spdlog::logger* GetAppLogger();

	private:
		Log();
		~Log() = default;

	private:
		static Log* s_pInstance;

		std::shared_ptr<spdlog::logger> m_pEngineLogger;
		std::shared_ptr<spdlog::logger> m_pAppLogger;
	};
}