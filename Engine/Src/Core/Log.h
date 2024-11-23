#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#define DV_LOG(CategoryName, Level, Format, ...) \
	do { \
		auto logger = Dive::LogManager::GetLogger(#CategoryName); \
		logger->log(spdlog::level::Level, Format, ##__VA_ARGS__); \
	} while (0)

namespace Dive
{
	class LogManager
	{
	public:
		static std::shared_ptr<spdlog::logger> GetLogger(const std::string& category);

		// �ݵ�� �ΰ� ��� ���� �����ؾ� �Ѵ�.
		static void SetLevels(spdlog::level::level_enum setLevel, spdlog::level::level_enum flushLevel) { s_SetLevel = setLevel; s_FlushLevel = flushLevel; }
		static void SetFilename(const std::string& filename) { s_Filename = filename; }
		// ��� Ȯ�ο�(���� �Ұ�)
		static const std::string& GetFilename() { return s_Filename; }

	private:
		static std::string s_Filename;
		static spdlog::level::level_enum s_SetLevel;
		static spdlog::level::level_enum s_FlushLevel;
	};
}