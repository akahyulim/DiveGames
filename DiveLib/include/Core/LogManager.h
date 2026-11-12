#pragma once
#include <string>
#include <format>

namespace Dive
{
	enum class eLogLevel
	{
		trace, debug, info, warn, err, critical, off
	};

	class LogManager
	{
	public:
		static void Shutdown();
		static void SetLevels(eLogLevel setLevel, eLogLevel flushLevel);
		static void SetFilename(const std::string& filename);
		static const std::string& GetFilename();

		template<typename... Args>
		static void LogMessage(const char* category, eLogLevel level, std::format_string<Args...> fmt, Args&&... args)
		{
			std::string formatted = std::format(fmt, std::forward<Args>(args)...);
			OutputInternal(category, level, formatted);
		}

	private:
		static void OutputInternal(const char* category, eLogLevel level, const std::string& message);
	};
}

#define DV_LOG(CategoryName, Level, Format, ...) \
	Dive::LogManager::LogMessage(#CategoryName, Dive::eLogLevel::Level, Format, ##__VA_ARGS__)